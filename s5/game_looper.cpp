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
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_2_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_2_");
		tp->GetToByPrefix(frames_magicWeapon, "weapon_");
	}
	ready = true;											// all tex ready

	camera.SetMaxFrameSize({32,32});
	camera.SetScale(4);

	heros.EmplaceShared()->Init(0, { -30, 30 });
	heros.EmplaceShared()->Init(1, { 30, 30 });
	heros.EmplaceShared()->Init(2, { 30, -30 });
	heros.EmplaceShared()->Init(3, { -30, -30 });

	for (int i = 0; i < 1000; ++i) {
		heroMagicWeapons.EmplaceShared()->Init(
			gLooper.rnd.Next<int>(gLooper.frames_magicWeapon.size() - 1),
			heros[rnd.Next<int>(0, 3)], 
			{});
		co_yield 0;
	}

	while (true) co_yield 0;								// do not quit
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;

	for (auto& o : heros) { o->mainLogic(); }
	for (auto& o : heroMagicWeapons) { o->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {
		for (auto& o : heros) { o->Draw(); }
		for (auto& o : heroMagicWeapons) { o->Draw(); }
		// todo: order by Y
	}
	fv.Draw(ctc72);											// show fps
}
