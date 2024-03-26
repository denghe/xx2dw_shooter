#pragma once
#ifdef __EMSCRIPTEN__
#include "engine_engine.h"
#else
#include "engine_engine_desktop.h"
#endif

/*****************************************************************/
/*****************************************************************/
// game_looper.h example:
/*


#pragma once
#include "engine.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;


*/
/*****************************************************************/
/*****************************************************************/
// pch.h example:
/*


#pragma once
#ifndef XX2DW_SHOOTER_DESKTOP_PCH_H_
#define XX2DW_SHOOTER_DESKTOP_PCH_H_

#include <engine.h>

#endif


*/
/*****************************************************************/
/*****************************************************************/
// pch.cpp example:
/*


#include <pch.h>
#include <game_looper.h>
#include <engine__pch_cpp.h>

GameLooper gLooper;

#ifdef __EMSCRIPTEN__
int32_t main() {
	gLooper.EngineInit();
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
#else
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_d1";
	gLooper.Init();
	gLooper.Run();
}
#endif


*/
