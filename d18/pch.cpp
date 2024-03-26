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
	gLooper.title = "xx2dw_d18";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif
