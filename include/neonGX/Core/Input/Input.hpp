/*
 * neonGX - Input.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_INPUT_H
#define NEONGX_INPUT_H

#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/ADT.hpp>
#include <string>
#include <memory>

namespace neonGX {

#ifdef NEONGX_USE_EMSCRIPTEN
  enum class KeyAction {
    KeyNone,
    KeyDown,
    KeyUp,
    KeyPress
  };

  enum KeyCode : uint16_t {
    KEY_None,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_ENTER,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KC_COUNT,
  };
#else
  static constexpr int KEY_A = GLFW_KEY_A;
  static constexpr int KEY_B = GLFW_KEY_B;
  static constexpr int KEY_C = GLFW_KEY_C;
  static constexpr int KEY_D = GLFW_KEY_D;
  static constexpr int KEY_E = GLFW_KEY_E;
  static constexpr int KEY_F = GLFW_KEY_F;
  static constexpr int KEY_G = GLFW_KEY_G;
  static constexpr int KEY_H = GLFW_KEY_H;
  static constexpr int KEY_I = GLFW_KEY_I;
  static constexpr int KEY_J = GLFW_KEY_J;
  static constexpr int KEY_K = GLFW_KEY_K;
  static constexpr int KEY_L = GLFW_KEY_L;
  static constexpr int KEY_M = GLFW_KEY_M;
  static constexpr int KEY_N = GLFW_KEY_N;
  static constexpr int KEY_O = GLFW_KEY_O;
  static constexpr int KEY_P = GLFW_KEY_P;
  static constexpr int KEY_Q = GLFW_KEY_Q;
  static constexpr int KEY_R = GLFW_KEY_R;
  static constexpr int KEY_S = GLFW_KEY_S;
  static constexpr int KEY_T = GLFW_KEY_T;
  static constexpr int KEY_U = GLFW_KEY_U;
  static constexpr int KEY_V = GLFW_KEY_V;
  static constexpr int KEY_W = GLFW_KEY_W;
  static constexpr int KEY_X = GLFW_KEY_X;
  static constexpr int KEY_Y = GLFW_KEY_Y;
  static constexpr int KEY_Z = GLFW_KEY_Z;
  static constexpr int KEY_ENTER = GLFW_KEY_ENTER;
  static constexpr int KEY_UP = GLFW_KEY_UP;
  static constexpr int KEY_DOWN = GLFW_KEY_DOWN;
#endif

  class InputHandle {
  private:
    std::string m_CanvasId;
    webgl_context_handle m_GlHandle;

#ifdef NEONGX_USE_EMSCRIPTEN
    KeyAction KeyState[KC_COUNT] = { KeyAction::KeyNone };
    NPoint m_MousePosition;
#endif

  public:
    InputHandle(webgl_context_handle glHandle, const std::string& canvasId);
    ~InputHandle();

#ifdef NEONGX_USE_EMSCRIPTEN
    void ProcessEvent(int eventType, const EmscriptenMouseEvent* mouseEvent);
    void ProcessEvent(int eventType, const EmscriptenKeyboardEvent* keyEvent,
                      KeyAction action);

    NPoint GetMousePosition() const {
      return m_MousePosition;
    }

    bool GetKey(KeyCode code) {
      return KeyState[code] == KeyAction::KeyDown;
    }

#else
    NPoint GetMousePosition() const {
      double xPos = 0;
      double yPos = 0;
      glfwGetCursorPos(m_GlHandle, &xPos, &yPos);
      return { int32_t(xPos), int32_t(yPos) };
    }

    bool GetKey(int keyCode) {
      return glfwGetKey(m_GlHandle, keyCode) == GLFW_PRESS;
    }

    bool GetKeyUp(int keyCode) {
      return glfwGetKey(m_GlHandle, keyCode) == GLFW_RELEASE;
    }
#endif
  };

} // end namespace neonGX

#endif // !NEONGX_INPUT_H
