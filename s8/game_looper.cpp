#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();											// font init
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		size_t n{};
		n = tp->GetToByPrefix(frames_button, "");
		xx_assert(n);
	}
	ready = true;											// all tex ready

}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;										// todo: show loading ?

	//buttons.Foreach([&](auto& o) {
	//	return o->mainLogic.Resume();
	//});
}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		//buttons.Foreach([&](auto& o) {
		//	if (gLooper.camera.InArea(o->pos)) {
		//		o->Draw();
		//	}
		//});
	}
	fv.Draw(ctc72);											// show fps
}
