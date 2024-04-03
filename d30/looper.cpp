#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"

Looper gLooper;
ResFrames& gRes(gLooper.res);

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
	gLooper.title = "xx2dw_d30";
	gLooper.Init();
	gLooper.Run<true>();
}
#endif

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::string picRoot = "d30_res/";
	co_await res.AsyncLoad(picRoot, 1024);

	// ui cfg init
	s9cfg.frame = res.ui_border;
	s9cfg.texScale = { 1, 1 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x55, 0x55, 0x55, 0xff };

	// space data init
	srdd.Init(128, 32);

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
