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
	printf("2\n");
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();											// font init
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		tp->GetToByPrefix(frames_pumpkin, "pumpkin_");
		tp->GetToByPrefix(frames_weapon, "weapon_");
	}
	ready = true;											// all tex ready

	camera.SetMaxFrameSize({32,32});
	camera.SetScale(2);

	heros.Emplace().Emplace()->Init(0, {}); 
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;

	heros.Foreach([&](auto& o) { return o->MainLogic.Resume(); });
}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		heros.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
	}
	fv.Draw(ctc72);											// show fps
}
