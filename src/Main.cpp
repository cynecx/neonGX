/*
 * neonGX - Main.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

/*
 * ~ NeoN Pong ~
 * Implementierung
 *
 */

#include <neonGX/Core/neonGX.hpp>
#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLRenderer.hpp>
#include <neonGX/Core/Text/FontTextureManager.hpp>
#include <neonGX/Core/Text/Text.hpp>
#include <neonGX/Core/Sprites/Sprite.hpp>
#include <neonGX/Core/Input/Input.hpp>
#include <neonGX/Core/Timer.hpp>
#include <neonGX/Core/ADT.hpp>

#include <iostream>
#include <unordered_map>
#include <random>
#include <thread>

#include <NeoNPong/pong.pb.h>
#include <boost/format.hpp>

#include <asio.hpp>


namespace {

  std::thread NetworkingThread;
  asio::io_service MainIOService;

  class NeonPong {
  private:
    enum class ColorScheme {
      Yellow,
      Orange,
      Violet,
      Blue
    };

    std::unique_ptr<neonGX::GLRenderer> renderer;
    std::unique_ptr<neonGX::InputHandle> inputHandle;

    std::unordered_map<std::string, std::string> SpriteFiles {
        { "field_blue", "./assets/sprites/field_blue.png" },
        { "field_violet", "./assets/sprites/field_violet.png" },
        { "field_orange", "./assets/sprites/field_orange.png" },
        { "field_yellow", "./assets/sprites/field_yellow.png" },
        { "ball_blue", "./assets/sprites/ball_blue.png" },
        { "ball_violet", "./assets/sprites/ball_violet.png" },
        { "ball_orange", "./assets/sprites/ball_orange.png" },
        { "ball_yellow", "./assets/sprites/ball_yellow.png" },
        { "paddle_blue", "./assets/sprites/paddle_blue.png" },
        { "paddle_violet", "./assets/sprites/paddle_violet.png" },
        { "paddle_orange", "./assets/sprites/paddle_orange.png" },
        { "paddle_yellow", "./assets/sprites/paddle_yellow.png" }
    };

    using BaseTexturePtr = std::shared_ptr<neonGX::BaseTexture>;
    using TexturePtr = std::shared_ptr<neonGX::Texture>;
    using SpritePtr = std::shared_ptr<neonGX::Sprite>;

    static constexpr int32_t PaddleMovement = 10;
    static constexpr int32_t SpaceBetweenPaddleAndWall = 25;

    std::unique_ptr<neonGX::Container> container;

    std::unordered_map<std::string, BaseTexturePtr> BaseTextureMap;
    std::unordered_map<std::string, TexturePtr> TextureMap;
    std::unordered_map<std::string, SpritePtr> SpriteMap;

    std::shared_ptr<neonGX::Text> BannerText;

    neonGX::FPoint CurrentBallVelocity{0.04f, 0.04f};
    neonGX::FPoint MaxBallVelocity{0.4f, 0.4f};
    float BallAcceleration = 0.04f;

    float BallDirectionX = 1.0f;
    float BallDirectionY = 1.0f;

    int ScorePlayer1 = 0;
    int ScorePlayer2 = 0;

    bool GameRunning = false;

    double PreviousClock = 0;
    double Lag = 0;

    std::random_device randomDevice;
    std::mt19937 rnGen;

    std::shared_ptr<neonGX::FontTextureManager> fontManager;

    neonGX::Timer ballDeltaTimer;

    ColorScheme CurrentColorScheme;

    template <typename T>
    using rngNum = std::uniform_int_distribution<T>;

    neonGX::optional<asio::ip::tcp::socket> mainSocket;
    std::vector<uint8_t> readBuffer;

  public:
    explicit NeonPong() {
      PreviousClock = GetCurrentClock();
      rnGen = std::mt19937(randomDevice());
      readBuffer.resize(1024 * 8);

      InitializeNeonGX();
      InitializeAllTextures();
      InitializeFontsAndTextObjects();
      InitializeSpritesAndField();
      InitializeServerConnection();
    }

    NeonPong(const NeonPong&) = delete;
    NeonPong& operator=(const NeonPong&) = delete;

    ~NeonPong() = default;

    neonGX::webgl_context_handle GetGLHandle() {
      return renderer->webgl_handle;
    }

    void InitializeServerConnection() {
      using namespace asio;

      ip::tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 8084);
      mainSocket = ip::tcp::socket(MainIOService);
      mainSocket->async_connect(endpoint, [this] (const asio::error_code& error) {
        handle_connect(error);
      });
    }

    void write_handler(const asio::error_code& ec,
                       std::size_t bytes_transferred) {
      std::cout << "Data transmitted" << std::endl;
    }

    void read_handler(const asio::error_code& ec,
                      std::size_t bytes_transferred) {
      if(ec == asio::error::eof || ec == asio::error::connection_reset) {
        std::cout << "Socket disconnected" << std::endl;
        return;
      }

      mainSocket->async_receive(asio::buffer(readBuffer.data(),
          readBuffer.size()),
          [this] (const asio::error_code& ec, std::size_t bytes_transferred) {
            read_handler(ec, bytes_transferred);
          }
      );

      if(bytes_transferred > 0) {
        std::string tmpData(reinterpret_cast<char*>(readBuffer.data()),
            bytes_transferred);

        pong::GamePayload payload;

        if(!payload.ParseFromString(tmpData)) {
          std::cout << "Received invalid payload." << std::endl;
          return;
        }

        if(payload.type() ==
           pong::GamePayload_Type::GamePayload_Type_Update) {
          auto& update = payload.update();

          auto ball = SpriteMap["ball"];
          ball->SetPosition({ float(update.ball_x()), float(update.ball_y()) });

          UpdateScoreServer(update.p1_points(), update.p2_points());

          GameRunning = update.started();

          auto player1 = SpriteMap["player1"];
          auto p1pos = player1->GetPosition();
          p1pos.y =  update.p1_dy();
          player1->SetPosition(p1pos);

          auto player2 = SpriteMap["player2"];
          auto p2pos = player2->GetPosition();
          p2pos.y =  update.p2_dy();
          player2->SetPosition(p2pos);
        }
      }
    }

    void handle_connect(const asio::error_code& error) {
      using namespace neonGX;

      std::cout << "Asio connected!" << std::endl;

      pong::GamePayload payload;
      payload.set_type(pong::GamePayload_Type::GamePayload_Type_Settings);

      FSize fieldSize = BaseTextureMap["field_blue"]->m_Size;
      FSize paddleSize = BaseTextureMap["paddle_blue"]->m_Size;
      FSize ballSize = BaseTextureMap["ball_blue"]->m_Size;

      auto* settings = new pong::GameSettings();
      settings->set_field_width(int32_t(fieldSize.width));
      settings->set_field_height(int32_t(fieldSize.height));
      settings->set_paddle_width(int32_t(paddleSize.width));
      settings->set_paddle_height(int32_t(paddleSize.height));
      settings->set_ball_width(int32_t(ballSize.width));
      settings->set_ball_height(int32_t(ballSize.height));

      payload.set_allocated_settings(settings);
      auto serialized = payload.SerializeAsString();

      mainSocket->write_some(
          asio::buffer(serialized.data(), serialized.size()));

      mainSocket->async_receive(asio::buffer(readBuffer.data(),
          readBuffer.size()),
          [this] (const asio::error_code& ec, std::size_t bytes_transferred) {
            read_handler(ec, bytes_transferred);
          }
      );
    }

    void InitializeFontsAndTextObjects() {
      using namespace neonGX;

      bool result = fontManager->LoadFont(
          "./assets/fonts/mh.ttf", "MinimalHard42", 55);
      assert(result);
      ((void)result);

      BannerText = std::make_shared<Text>("", "MinimalHard42");

      UpdateScore(true, false);
    }

    void InitializeNeonGX() {
      using namespace neonGX;
      using namespace std::string_literals;

      RendererSettings settings;
      settings.BackgroundColor = ColorRGB::FromHex("#ffffff").value();
      settings.Transparent = false;

      renderer = std::make_unique<GLRenderer>("canvas0"s,
          neonGX::FSize{1280, 720}, settings);
      inputHandle = std::make_unique<neonGX::InputHandle>(
          renderer->webgl_handle, "canvas0");
      fontManager = renderer->GetFontTextureManager();
    }

    void InitializeSpritesAndField() {
      using namespace neonGX;

      SpriteMap.insert({ "field", std::make_shared<neonGX::Sprite>(
          TextureMap["field_blue"]) });
      SpriteMap.insert({ "ball", std::make_shared<neonGX::Sprite>(
          TextureMap["ball_blue"]) });
      SpriteMap.insert({ "player1", std::make_shared<neonGX::Sprite>(
          TextureMap["paddle_blue"]) });
      SpriteMap.insert({ "player2", std::make_shared<neonGX::Sprite>(
          TextureMap["paddle_blue"]) });

      ResetGame();

      container = std::make_unique<Container>();
      container->AddChild(SpriteMap["field"]);
      container->AddChild(BannerText);
      container->AddChild(SpriteMap["ball"]);
      container->AddChild(SpriteMap["player1"]);
      container->AddChild(SpriteMap["player2"]);
    }

    void InitializeAllTextures() {
      using namespace neonGX;

      for(auto& it : SpriteFiles) {
        auto baseTexture = std::make_shared<neonGX::BaseTexture>();
        BaseTextureMap.insert({it.first, baseTexture});

        auto ptrImg = LoadPNGImageShared(it.second);
        baseTexture->SetImage(ptrImg);

        TextureMap.insert({it.first,
                           std::make_shared<Texture>(baseTexture, nullopt)});
      }
    }

    void ChangeColors() {
      std::array<std::string, 4> Colors{
          { "blue", "yellow", "violet", "orange" }};

      int colorIndx = rngNum<int32_t>(0, 3)(rnGen);
      std::string color = Colors[colorIndx];

      switch(colorIndx) {
        case 0:
          CurrentColorScheme = ColorScheme::Blue;
          break;
        case 1:
          CurrentColorScheme = ColorScheme::Yellow;
          break;
        case 2:
          CurrentColorScheme = ColorScheme::Violet;
          break;
        case 3:
          CurrentColorScheme = ColorScheme::Orange;
          break;
        default:
          assert("What da fuck is dis shit..." && false);
      }

      auto& colorRefText = BannerText->GetColorRef();

      switch(CurrentColorScheme) {
        case ColorScheme::Orange:
          colorRefText = *neonGX::ColorRGB::FromHex("#ffbb03");
          break;
        case ColorScheme::Violet:
          colorRefText = *neonGX::ColorRGB::FromHex("#d33c79");
          break;
        case ColorScheme::Yellow:
          colorRefText = *neonGX::ColorRGB::FromHex("#f9df00");
          break;
        case ColorScheme::Blue:
          colorRefText = *neonGX::ColorRGB::FromHex("#009cff");
          break;
      }

      SpriteMap["field"]->SetTexture(TextureMap["field_" + color]);
      SpriteMap["ball"]->SetTexture(TextureMap["ball_" + color]);
      SpriteMap["player1"]->SetTexture(TextureMap["paddle_" + color]);
      SpriteMap["player2"]->SetTexture(TextureMap["paddle_" + color]);
    }

    void ResetGame() {
      using namespace neonGX;

      FSize fieldSize = BaseTextureMap["field_blue"]->m_Size;
      FSize paddleSize = BaseTextureMap["paddle_blue"]->m_Size;
      FSize ballSize = BaseTextureMap["ball_blue"]->m_Size;

      SpriteMap["ball"]->SetPosition({
          fieldSize.width / 2 - ballSize.width / 2,
          fieldSize.height / 2 - ballSize.height / 2 });

      float centerPosPaddle = fieldSize.height / 2 - paddleSize.height / 2;

      SpriteMap["player1"]->SetPosition({
          SpaceBetweenPaddleAndWall,
          centerPosPaddle});

      SpriteMap["player2"]->SetPosition({
          fieldSize.width - paddleSize.width - SpaceBetweenPaddleAndWall,
          centerPosPaddle });

      ChangeColors();
    }

    void UpdateScoreServer(int32_t p1, int32_t p2) {
      using namespace neonGX;

      ScorePlayer1 = p1;
      ScorePlayer2 = p2;

      auto scoreStr = boost::format("%1%        %2%")
                      % ScorePlayer1 % ScorePlayer2;
      BannerText->SetText(scoreStr.str());

      FSize fieldSize = BaseTextureMap["field_blue"]->m_Size;
      FSize bannerSize = BannerText->GetSize(renderer.get());

      BannerText->SetPosition({
          fieldSize.width / 2 - bannerSize.width / 2,
          20.0f,
      });
    }

    void UpdateScore(bool Init, bool Player1) {
      using namespace neonGX;

      if(!Init) {
        if(Player1) {
          ScorePlayer1++;
        } else {
          ScorePlayer2++;
        }
      } else {
        ScorePlayer1 = ScorePlayer2 = 0;
      }

      auto scoreStr = boost::format("%1%        %2%")
                      % ScorePlayer1 % ScorePlayer2;
      BannerText->SetText(scoreStr.str());

      FSize fieldSize = BaseTextureMap["field_blue"]->m_Size;
      FSize bannerSize = BannerText->GetSize(renderer.get());

      BannerText->SetPosition({
          fieldSize.width / 2 - bannerSize.width / 2,
          20.0f,
      });
    }

    void HandleBall() {
      auto dtime = ballDeltaTimer.ElapsedAndReset<float>();

      auto ball = SpriteMap["ball"];

      auto fieldSize = BaseTextureMap["field_blue"]->m_Size.To<int32_t>();

      auto ballSize = ball->GetSize();
      auto currBallPos = ball->GetPosition();

      auto oldVelocity = CurrentBallVelocity;
      CurrentBallVelocity.x = std::min<float>(
          oldVelocity.x + BallAcceleration * dtime, MaxBallVelocity.x);
      CurrentBallVelocity.y = std::min<float>(
          oldVelocity.y + BallAcceleration * dtime, MaxBallVelocity.y);

      currBallPos.x += BallDirectionX * (dtime * (oldVelocity.x + CurrentBallVelocity.x) / 2);
      currBallPos.y += BallDirectionY * (dtime * (oldVelocity.y + CurrentBallVelocity.y) / 2);

      if(currBallPos.x + ballSize.width >= fieldSize.width) {
        GameRunning = false;
        UpdateScore(false, true);
      } else if(currBallPos.x <= 0) {
        GameRunning = false;
        UpdateScore(false, false);
      }

      if(!GameRunning) {
        ResetGame();
        return;
      }

      if(currBallPos.y <= 0) {
        BounceBall(0, 1);
      } else if(currBallPos.y + ballSize.width >= fieldSize.height) {
        BounceBall(0, -1);
      }

      auto player1 = SpriteMap["player1"];
      auto player2 = SpriteMap["player2"];

      auto p1Size = player1->GetSize().To<int32_t>();
      auto p2Size = player1->GetSize().To<int32_t>();

      auto currPosP1 = player1->GetPosition();
      auto currPosP2 = player2->GetPosition();

      neonGX::NRectangle ballRec(currBallPos, ballSize.To<int32_t>());
      neonGX::NRectangle paddle1(currPosP1, p1Size);
      neonGX::NRectangle paddle2(currPosP2, p2Size);

      if(paddle1.Intersect(ballRec)) {
        BounceBall(1.02f, 0);
      }

      if(paddle2.Intersect(ballRec)) {
        BounceBall(-1.02f, 0);
      }

      ball->SetPosition(currBallPos);
    }

    void BounceBall(float mulX, float mulY) {
      if(mulX != 0) {
        BallDirectionX = std::fabs(BallDirectionX) * mulX;
      }

      if(mulY != 0) {
        BallDirectionY = std::fabs(BallDirectionY) * mulY;
      }
    }

    void HandlePaddle() {
      auto player1 = SpriteMap["player1"];
      auto player2 = SpriteMap["player2"];

      auto p1Size = player1->GetSize().To<int32_t>();
      auto p2Size = player1->GetSize().To<int32_t>();

      auto currPosP1 = player1->GetPosition();
      auto currPosP2 = player2->GetPosition();

      auto fieldSize = BaseTextureMap["field_blue"]->m_Size.To<int>();

      int32_t distance = PaddleMovement;

      if(inputHandle->GetKey(neonGX::KEY_S)) {
        if(currPosP1.y + p1Size.height + distance > fieldSize.height) {
          currPosP1.y = fieldSize.height - p1Size.height;
        } else {
          currPosP1.y += distance;
        }
      }

      if(inputHandle->GetKey(neonGX::KEY_W)) {
        if(currPosP1.y - distance < 0) {
          currPosP1.y = 0;
        } else {
          currPosP1.y -= distance;
        }
      }

      if(inputHandle->GetKey(neonGX::KEY_UP)) {
        if(currPosP2.y - distance < 0) {
          currPosP2.y = 0;
        } else {
          currPosP2.y -= distance;
        }
      }

      if(inputHandle->GetKey(neonGX::KEY_DOWN)) {
        if(currPosP2.y + p2Size.height + distance > fieldSize.height) {
          currPosP2.y = fieldSize.height - p2Size.height;
        } else {
          currPosP2.y += distance;
        }
      }

      player1->SetPosition(currPosP1);
      player2->SetPosition(currPosP2);
    }

    void SetRandomBallDirection() {
      auto sign = rngNum<int32_t>(0, 1);
      BounceBall((sign(rnGen) ? 1 : -1), (sign(rnGen) ? 1 : -1));
    }

    void DoGameLogicNoServer() {
      if(!GameRunning) {
        if(inputHandle->GetKey(neonGX::KEY_ENTER)) {
          ballDeltaTimer.Reset();
          GameRunning = true;
          SetRandomBallDirection();
        }
      }

      if(GameRunning) {
        HandlePaddle();
        HandleBall();
      }
    }

    void DoGameLogic() {
      // Server Pull settings (Through rpc...)
    }

    long GetCurrentClock() {
      using namespace std::chrono;
      return duration_cast<milliseconds>(
          high_resolution_clock::now().time_since_epoch()).count();
    }

    double FrameRate = 1000 / 60;
    double LeftOvers = 0;

    void MainLoop() {
      DoGameLogicNoServer();
      renderer->Render(container.get());
//      auto currentClock = GetCurrentClock();
//      auto elapsedTime = (currentClock - PreviousClock) + LeftOvers;
//      auto catchUpFrameCount = uint32_t(std::floor(elapsedTime / FrameRate));
//
//      for(uint32_t i = 0; i < catchUpFrameCount; i++) {
//        DoGameLogic();
//      }
//
//      LeftOvers = elapsedTime - (catchUpFrameCount * FrameRate);
//      PreviousClock = currentClock;
    }
  };

  std::unique_ptr<NeonPong> gameInstance;

  void GameLoop() {
    // TODO: MainIOService.poll_one();
    gameInstance->MainLoop();
  }

  void emSocketCallback(int fd, void *userData) {
    MainIOService.poll_one();
  }

  void emSocketErrorCallback(int fd, int err, const char* msg, void *userData) {
    MainIOService.poll_one();
  }

  void SetupEmscriptenSockets() {
//    emscripten_set_socket_connection_callback(nullptr, emSocketCallback);
//    emscripten_set_socket_open_callback(nullptr, emSocketCallback);
//    emscripten_set_socket_close_callback(nullptr, emSocketCallback);
//    emscripten_set_socket_listen_callback(nullptr, emSocketCallback);
//    emscripten_set_socket_message_callback(nullptr, emSocketCallback);
//    emscripten_set_socket_error_callback(nullptr, emSocketErrorCallback);
  }

}

int main() {
  neonGX::InitializeEngine();

  //SetupEmscriptenSockets();
  std::srand(std::time(0));

  gameInstance = std::make_unique<NeonPong>();

  neonGX::RunMainLoop(gameInstance->GetGLHandle(), GameLoop);

  return 0;
}
