#pragma once
#include "engine_includes.h"
#include "engine_gdesign.h"
#include "engine_rnd.h"
#include "engine_curvemovepath.h"
#include "engine_spacegrid_box.h"
#include "engine_spacegrid_circle.h"
#include "engine_spacegrid_ringdiffuse.h"

// type same as EmscriptenKeyboardEvent.what
using KeyboardKeys_t = decltype(EmscriptenKeyboardEvent::which);
enum class KeyboardKeys : KeyboardKeys_t {
    Unknown = 0,
    A = 65,
    B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
    , MAX_VALUE
};

enum class MouseEvents : int {
    Unknown = 0, Down, Move, Up, Cancel
};

struct Mouse {
    XY pos{};
    std::array<bool, 16> btnStates{};
    //MouseEvents event{};
    //void* eventHandler{};
    //void ClearEvent() { event = MouseEvents::Unknown; }
    //void ClearEventAndSetHandler(void* h) { event = MouseEvents::Unknown; eventHandler = h; }
};

struct MouseEventHandlerNode;

struct Shader;
struct EngineBase0 {
    xx::Tasks tasks;
    Rnd rnd;

    float fps{};    // cache
    float windowWidth = 800, windowHeight = 600;          // can change at Init()
    float windowWidth_2 = windowWidth / 2, windowHeight_2 = windowHeight / 2;
    float flipY{ 1 };   // -1: flip  for ogl frame buffer
    Shader* shader{};

    // events
    xx::Weak<MouseEventHandlerNode> mouseEventHandler{};	// current( focus )
    SpaceGridAB<MouseEventHandlerNode, XY> mouseEventHandlers;	// mouse down event receivers

    std::array<bool, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysStates{};
    std::array<double, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysDelays{};

    Mouse mouse;
    bool touchMode{};

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };


    EngineBase0();
};

inline EngineBase0* gEngine{};

inline EngineBase0::EngineBase0() {
    gEngine = this;
}
