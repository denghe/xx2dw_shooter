﻿#include "pch.h"

xx::Task<> GameLooper::MainTask() {
	{
#ifdef __EMSCRIPTEN__
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
#else
		auto tp = LoadTexturePacker("res/dungeon.blist");
#endif
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

	heros.Emplace().Emplace()->Init(0, { -120, 120 }); 
	heros.Emplace().Emplace()->Init(1, { 120, 120 });
	heros.Emplace().Emplace()->Init(2, { 120, -120 });
	heros.Emplace().Emplace()->Init(3, { -120, -120 });

	tasks.Add([&]()->xx::Task<> {
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; ++j) {
				heroMagicWeapons.Emplace().Emplace()->Init(
					rnd.Next<int>((int)frames_magicWeapon.size() - 1), 
					heros[0], 
					{ rnd.Next<float>(-80, 80), rnd.Next<float>(-80, 80)
				});
			}
			co_yield 0;
		}

		co_await AsyncSleep(2);

		for (int i = 0, ie = heroMagicWeapons.len; i < ie; i++) {
			heroMagicWeapons[i]->target = heros[(i % 3) + 1];
			if (i % 10 == 0) co_yield 0;
		}
	});

	//heroMagicWeapons.Emplace().Emplace()->Init(
	//	rnd.Next<int>(frames_magicWeapon.size() - 1), 
	//	heros[0], 
	//	{ rnd.Next<float>(-80, 80), rnd.Next<float>(-80, 80)
	//});
	//co_await AsyncSleep(2);
	//heroMagicWeapons[0]->target = heros[1];


	while (true) {
		heros[1]->body.colorplus = 100000;
		heros[2]->body.colorplus = 100000;
		heros[3]->body.colorplus = 100000;
		co_await AsyncSleep(0.05);
		heros[1]->body.colorplus = 1;
		heros[2]->body.colorplus = 1;
		heros[3]->body.colorplus = 1;
		co_await AsyncSleep(0.05);
	}
}

void GameLooper::Update() {
	if (KeyDownDelay(KeyboardKeys::Z, 0.02f)) {				// zoom control
		camera.DecreaseScale(0.02f, 0.02f);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.IncreaseScale(0.02f, 5);
	}
	if (!ready) return;

	heros.Foreach([&](auto& o) {
		afterimages.Emplace().Emplace()->Init(*o);
		return o->mainLogic.Resume();
	});

	heroMagicWeapons.Foreach([&](auto& o) {
		if (!o->following) {
			afterimages.Emplace().Emplace()->Init(*o);
		}
		return o->mainLogic.Resume();
	});

	afterimages.Foreach([&](auto& o) { return o->mainLogic.Resume(); });
}

void GameLooper::Draw() {
	if (!ready) return;
	camera.Calc();

	afterimages.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});

	heros.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			ysprites.Add(YSprite{ o->pos.y, o.pointer });
		}
	});
	heroMagicWeapons.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			ysprites.Add(YSprite{ o->pos.y - 3, o.pointer });
		}
	});

	std::sort(ysprites.buf, ysprites.buf + ysprites.len, 
		[](YSprite const& a, YSprite const& b) {
			return a.y < b.y;
		});
	for (int i = 0, ie = ysprites.len; i < ie; ++i) {
		ysprites[i].s->Draw();
	}
	ysprites.Clear();
}
