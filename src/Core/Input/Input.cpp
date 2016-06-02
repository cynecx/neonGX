/*
 * neonGX - Input.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Input/Input.hpp>
#include <iostream>
#include <unordered_map>

namespace neonGX {

#ifdef NEONGX_USE_EMSCRIPTEN
  static EM_BOOL emMouseCallback(int eventType,
                                 const EmscriptenMouseEvent* mouseEvent,
                                 void* userData) {
    assert(userData != nullptr);

    InputHandle* handler = reinterpret_cast<InputHandle*>(userData);
    handler->ProcessEvent(eventType, mouseEvent);

    return EM_FALSE;
  }

  static EM_BOOL emKeyCallback(int eventType,
                               const EmscriptenKeyboardEvent* mouseEvent,
                               void* userData) {
    assert(userData != nullptr);

    InputHandle* handler = reinterpret_cast<InputHandle*>(userData);
    if(eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
      handler->ProcessEvent(eventType, mouseEvent, KeyAction::KeyDown);
    } else if(eventType == EMSCRIPTEN_EVENT_KEYUP) {
      handler->ProcessEvent(eventType, mouseEvent, KeyAction::KeyUp);
    }

    return EM_FALSE;
  }
#endif

  InputHandle::InputHandle(webgl_context_handle glHandle,
                           const std::string& canvasId)
      : m_CanvasId(canvasId), m_GlHandle(glHandle) {
#ifdef NEONGX_USE_EMSCRIPTEN
    int result = emscripten_set_keydown_callback(0, this, 0, emKeyCallback);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);

    result = emscripten_set_keyup_callback(0, this, 0, emKeyCallback);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);

    result = emscripten_set_mousemove_callback(m_CanvasId.c_str(),
        this, 0, emMouseCallback);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);
#endif
  }

  InputHandle::~InputHandle() {
#ifdef NEONGX_USE_EMSCRIPTEN
    int result = emscripten_set_mousemove_callback(m_CanvasId.c_str(),
        this, 0, nullptr);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);

    result = emscripten_set_keydown_callback(0, this, 0, nullptr);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);

    result = emscripten_set_keyup_callback(0, this, 0, nullptr);
    assert(result == EMSCRIPTEN_RESULT_SUCCESS);
#endif
  }

#ifdef NEONGX_USE_EMSCRIPTEN
  void InputHandle::ProcessEvent(int eventType,
                                 const EmscriptenMouseEvent* mouseEvent) {
    m_MousePosition.x = mouseEvent->targetX;
    m_MousePosition.y = mouseEvent->targetY;
  }

  void InputHandle::ProcessEvent(int eventType,
                                 const EmscriptenKeyboardEvent* keyEvent,
                                 KeyAction action) {
    uint16_t keyMap = KEY_None;

    if(keyEvent->keyCode >= 'A' && keyEvent->keyCode <= 'Z') {
      keyMap = uint16_t(keyEvent->keyCode) - uint16_t('A');
      keyMap += 1;
    } else if(keyEvent->keyCode == 38) {
      keyMap = KEY_UP;
    } else if(keyEvent->keyCode == 40) {
      keyMap = KEY_DOWN;
    } else if(keyEvent->keyCode == 37) {
      keyMap = KEY_LEFT;
    } else if(keyEvent->keyCode == 39) {
      keyMap = KEY_RIGHT;
    } else if(keyEvent->keyCode == 13) {
      keyMap = KEY_ENTER;
    }

    if(keyMap != KEY_None) {
      KeyState[keyMap] = action;
    }
  }

#endif

}
