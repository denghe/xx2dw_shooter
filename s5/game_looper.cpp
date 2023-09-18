﻿#include "pch.h"

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
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_2_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_2_");
		tp->GetToByPrefix(frames_magicWeapon, "weapon_");
	}
	ready = true;											// all tex ready

	camera.SetMaxFrameSize({32,32});
	camera.SetScale(2);

	heros.EmplaceShared()->Init(0, { -120, 120 }); 
	heros.EmplaceShared()->Init(1, { 120, 120 });
	heros.EmplaceShared()->Init(2, { 120, -120 });
	heros.EmplaceShared()->Init(3, { -120, -120 });

	for (int i = 0; i < 500; ++i) {
		for (int j = 0; j < 100; ++j) {
			heroMagicWeapons.EmplaceShared()->Init(
				rnd.Next<int>(frames_magicWeapon.size() - 1),
				heros[rnd.Next<int>(0, 3)],
				{ rnd.Next<float>(-80, 80), rnd.Next<float>(-80, 80) }
			);
		}
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
		for (auto& o : heroMagicWeapons) {
			if (!o->owner || o->pos.y - 3 <= o->owner->pos.y) o->Draw();
		}
		for (auto& o : heros) { o->Draw(); }
		for (auto& o : heroMagicWeapons) {
			if (!(!o->owner || o->pos.y - 3 <= o->owner->pos.y)) o->Draw();
		}
		// todo: order by Y with all monsters
	}
	fv.Draw(ctc72);											// show fps
}