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
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		size_t n{};
		n = tp->GetToByPrefix(frames_number_outlined, DamageNumber::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_blood_4, Blood::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_pumpkin, Hero_Pumpkin::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_weapon, Weapon::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_eye_fire, Bullet_EyeFire::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_eye_fire_blue, Bullet_EyeFireBlue::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_fireball_10, Bullet_Fireball::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_explosion, Bullet_Explosion::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_icon_gem, Experience::cResPrefix);
		xx_assert(n);
		n = tp->GetToByPrefix(frames_dragon_babywhite, Monster_Dragon_BabyWhite::cResPrefix);
		xx_assert(n);
	}
	ready = true;											// all tex ready

	sgrdd.Init(gGridNumRows, gGridCellDiameter);
	monstersGrid.Init(gGridNumRows, gGridNumCols, gGridCellDiameter);
	experiencesGrid.Init(gGridNumRows, gGridNumCols, gGridCellDiameter);

	XY ori{ (float)monstersGrid.maxX / 2 , (float)monstersGrid.maxY / 2 };

	camera.SetOriginal(ori);
	camera.SetMaxFrameSize({32,32});
	camera.SetScale(2);

	player1.Emplace();

	//heros.Emplace().Emplace<Hero_Pumpkin>()->Init(player1, ori + XY{-100, 0});
	//while(true) {
	//	for (int i = 0; i < 100; ++i) {
	//		Monster::CreateTo<Monster_Dragon_BabyWhite>(monsters)->Init(100, ori + XY{
	//			rnd.Next<float>(0, 100), rnd.Next<float>(-100, 100)
	//		});
	//	}
	//	co_await AsyncSleep(1);
	//}

	heros.Emplace().Emplace<Hero_Pumpkin>()->Init(player1, ori + XY{ 0, 0 });
	while (true) {
		for (size_t i = 0; i < 1; i++) {
			auto a = rnd.Next<float>(M_PI * 2);
			auto r = rnd.Next<float>(280, 330);
			Monster::CreateTo<Monster_Dragon_BabyWhite>(monsters)->Init(rnd.Next<int>(10, 50), ori + XY{std::cos(a), std::sin(a)} *r);
		}
		co_yield 0;
		co_yield 0;
	}

}

void GameLooper::Update() {
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;										// todo: show loading ?

	heros.Foreach([&](xx::Shared<Hero> const& o) {
		afterimages.Emplace().Emplace()->Init(*o->weapon);
		return o->Update();
	});

	bullets.Foreach([&](auto& o) {
		//afterimages.Emplace().Emplace()->Init(*o);
		return o->mainLogic.Resume();
	});

	monsters.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});

	afterimages.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});

	damageNumbers.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});

	bloods.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});

	flyingExperiences.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});
}

void GameLooper::Draw() {
	if (!ready) return;
	camera.Calc();

	experiences.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});

	flyingExperiences.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});

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

	bloods.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});

	bullets.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});

	damageNumbers.Foreach([&](auto& o) {
		if (gLooper.camera.InArea(o->pos)) {
			o->Draw();
		}
	});
}
