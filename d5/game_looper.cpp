#include "pch.h"

xx::Task<> GameLooper::MainTask() {
	{
#ifdef __EMSCRIPTEN__
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/d20.blist");
#else
		auto tp = LoadTexturePacker("res/d20.blist");
#endif
		xx_assert(tp);
		tp->GetToByPrefix(frames_heros.emplace_back(), "human_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "human_2_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "human_3_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "human_4_");
		tp->GetToByPrefix(frames_magicWeapon, "sword_");
	}
	ready = true;											// all tex ready

	camera.SetMaxFrameSize({32,32});
	camera.SetScale(2);

	heros.Emplace().Emplace()->Init(0, { -120, 120 }); 
	heros.Emplace().Emplace()->Init(1, { 120, 120 });
	heros.Emplace().Emplace()->Init(2, { 120, -120 });
	heros.Emplace().Emplace()->Init(3, { -120, -120 });

	for (int i = 0; i < 500; ++i) {
		for (int j = 0; j < 100; ++j) {
			heroMagicWeapons.Emplace().Emplace()->Init(
				rnd.Next<int>((int)frames_magicWeapon.size() - 1),
				heros[rnd.Next<int>(0, 3)],
				{ rnd.Next<float>(-80, 80), rnd.Next<float>(-80, 80) }
			);
		}
		co_yield 0;
	}

}

void GameLooper::Update() {
	if (KeyDownDelay(KeyboardKeys::Z, 0.02f)) {				// zoom control
		camera.DecreaseScale(0.02f, 0.02f);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.IncreaseScale(0.02f, 5);
	}
	if (!ready) return;

	for (auto& o : heros) { o->mainLogic(); }
	for (auto& o : heroMagicWeapons) { o->mainLogic(); }
}

void GameLooper::Draw() {
	if (!ready) return;
	for (auto& o : heroMagicWeapons) {
		if (!o->owner || o->pos.y - 3 <= o->owner->pos.y) o->Draw();
	}
	for (auto& o : heros) { o->Draw(); }
	for (auto& o : heroMagicWeapons) {
		if (!(!o->owner || o->pos.y - 3 <= o->owner->pos.y)) o->Draw();
	}
	// todo: order by Y with all monsters
}
