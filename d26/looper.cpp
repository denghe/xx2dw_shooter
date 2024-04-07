#include "pch.h"
#include "looper.h"
#include "scene_test1.h"

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
	gLooper.title = "xx2dw_d26";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


xx::Task<> Looper::MainTask() {
	sgrdd.Init(128, 32);

	co_await res.AsyncLoad("res/", 128);

	s9cfg_btn.frame = res.button;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	// load first scene
	co_await AsyncSwitchTo<SceneTest1>();
}
