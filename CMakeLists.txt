# ============================================================================
# ****************************************************************************
#   ~ neonGX ~ CMakeFile.txt ~
# ****************************************************************************
# ============================================================================

if(NEONGX_BUILD_USING_EMSCRIPTEN)
  set(CMAKE_TOOLCHAIN_FILE "/home/cynecx/dev/emscripten_dev/emscripten/cmake/Modules/Platform/Emscripten.cmake")
endif()

cmake_minimum_required(VERSION 3.5)
project(neonGX CXX)

if(NOT NEONGX_BUILD_USING_EMSCRIPTEN)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW REQUIRED glfw3)
endif()

file(GLOB_RECURSE SRC_LIST_CORE
    "${CMAKE_SOURCE_DIR}/src/*.cpp")

list(APPEND SRC_LIST ${SRC_LIST_CORE}
    "${CMAKE_SOURCE_DIR}/rpc/NeoNPong/pong.pb.cc")

add_executable(neonGX ${SRC_LIST})

target_include_directories(neonGX PUBLIC
    "${CMAKE_SOURCE_DIR}/include")

target_include_directories(neonGX SYSTEM PUBLIC
    "${CMAKE_SOURCE_DIR}/rpc/"
    "${CMAKE_SOURCE_DIR}/third-party/protobuf/src/"
    "${CMAKE_SOURCE_DIR}/third-party/freetype2/include"
    "${CMAKE_SOURCE_DIR}/third-party/asio/asio/include"
    "${CMAKE_SOURCE_DIR}/third-party/"
    "${GLFW_INCLUDE_DIRS}"
    )

if(NEONGX_BUILD_USING_EMSCRIPTEN)
  set(CMAKE_CXX_LINK_FLAGS "-s USE_PTHREADS=1 -s USE_FREETYPE=1 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s DEMANGLE_SUPPORT=1 -s EXCEPTION_DEBUG=1 --preload-file \"./assets\" -s FULL_ES2=1 --bind -s TOTAL_MEMORY=33554432 -fno-strict-aliasing ${CMAKE_SOURCE_DIR}/third-party/protobuf/src/.libs/libprotobuf.so")
  target_compile_definitions(neonGX PRIVATE NEONGX_USE_EMSCRIPTEN)
else()
  target_compile_definitions(neonGX PRIVATE GLFW_INCLUDE_ES2)
  target_link_libraries(neonGX
      glfw ${GLFW_LIBRARIES}
      png
      GL
      protobuf
      freetype)
endif()

target_compile_options(neonGX PRIVATE
    -std=c++1z
    -Wall
    -pedantic
    -fno-strict-aliasing)

add_custom_command(TARGET neonGX PRE_BUILD
    COMMAND "${CMAKE_SOURCE_DIR}/scripts/postbuild.sh")
