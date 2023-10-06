#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

xx::Task<> GameLooper::MainTask() {
	ls.FillCirclePoints({}, 100);
	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
	ls.Draw();
}
