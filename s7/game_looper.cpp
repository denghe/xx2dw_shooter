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
		n = tp->GetToByPrefix(frames_number_outlined, DamageNumber::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_pumpkin, Hero_Pumpkin::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_weapon, Weapon::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_eye_fire, Bullet_EyeFire::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_dragon_babywhite, Monster_Dragon_BabyWhite::cResPrefix);
		xx_assert(n);
	}
	ready = true;											// all tex ready

	monstersGrid.Init(gGridNumRows, gGridNumCols, gGridCellDiameter);
	sgrdd.Init(gGridNumRows, gGridCellDiameter);

	camera.SetMaxFrameSize({32,32});
	camera.SetScale(2);

	player1.Emplace();

	heros.Emplace().Emplace<Hero_Pumpkin>()->Init(player1, {});

	while(true) {
		Monster::CreateTo<Monster_Dragon_BabyWhite>(monsters)->Init(100, {100, 100});
		co_await AsyncSleep(10);
	}
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;										// todo: show loading ?

	heros.Foreach([&](xx::Shared<Hero> const& o) {
		//afterimages.Emplace().Emplace()->Init(*o->weapon);
		return o->update(o);
	});

	bullets.Foreach([&](auto& o) {
		//afterimages.Emplace().Emplace()->Init(*o);
		return o->update(o);
	});

	monsters.Foreach([&](auto& o) {
		return o->update(o);
	});

	afterimages.Foreach([&](auto& o) {
		return o->update(o);
	});
}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		heros.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
		afterimages.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
		heros.Foreach([&](auto& h) {
			auto& o = h->weapon;
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
		monsters.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
		bullets.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
	}
	fv.Draw(ctc72);											// show fps
}
