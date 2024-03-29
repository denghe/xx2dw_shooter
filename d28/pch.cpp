#include <pch.h>
#include <engine__pch_cpp.h>

#include "looper.h"

Looper gLooper;

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
	gLooper.title = "xx2dw_d28";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif
