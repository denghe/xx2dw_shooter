#include "pch.h"

void GameLooper::Init() {
	windowWidth = gDesign.width;
	windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
	// preload texs
    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");
	xx_assert(tp);

	tp->GetToByPrefix(frames_monster_1, "ma");
	tp->GetToByPrefix(frames_monster_2, "mb");
	tp->GetToByPrefix(frames_monster_3, "mc");
	tp->GetToByPrefix(frames_explosion, "e");
	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");			// ready flag

	sgc.Init(gGridNumRows, gGridNumCols, gGridCellDiameter);	// init physics grid
	sgrdd.Init(gGridNumRows, gGridCellDiameter);

	shooter.Emplace()->Init();	// make player char

	// generate monsters

//// cake
//printf("sgrdd.idxs.len = %d\n", (int)sgrdd.idxs.len);
//int i = 0;
//for (auto& [n, r] : sgrdd.lens) {
//	//printf("n = %d\n", n);
//	for (; i < n; ++i) {
//		//printf("idx = %d %d\n", idxs[i].x, idxs[i].y);
//		NewMonster<Monster1>(sgrdd.idxs[i].As<float>() * 2);
//	}
//	co_yield 0;
//}

//// doughnut
//tasks.Add([this]()->xx::Task<> {
//	while (true) {
//		for (size_t i = 0; i < 7; i++) {
//			auto a = rnd.Next<float>(M_PI * 2);
//			auto r = rnd.Next<float>(256, 384);
//			NewMonster<Monster1>(XY{ std::cos(a), std::sin(a) } * r);
//		}
//		co_yield 0;
//	}
//});
//tasks.Add([this]()->xx::Task<> {
//	while (true) {
//		for (size_t i = 0; i < 5; i++) {
//			auto a = rnd.Next<float>(M_PI * 2);
//			auto r = rnd.Next<float>(128, 256);
//			NewMonster<Monster2>(XY{ std::cos(a), std::sin(a) } *r);
//		}
//		co_yield 0;
//	}
//});
//tasks.Add([this]()->xx::Task<> {
//	while (true) {
//		for (size_t i = 0; i < 3; i++) {
//			auto a = rnd.Next<float>(M_PI * 2);
//			auto r = rnd.Next<float>(64, 128);
//			NewMonster<Monster3>(XY{ std::cos(a), std::sin(a) } *r);
//		}
//		co_yield 0;
//	}
//});

	while (true) {
		for (size_t i = 0; i < 30; i++) {
			auto a = rnd.Next<float>(M_PI * 2);
			auto r = rnd.Next<float>(384, 400);
			NewMonster<Monster1>(XY{ std::cos(a), std::sin(a) } *r);
		}
		co_yield 0;
	}
}

void GameLooper::Update() {
	if (!frame_shooter) return;

	if (shooter) {
		shooter();
	}
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });
	bullets_shooter2.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_explosion.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_damageText.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (!frame_shooter) return;

	monsters.Foreach([&](auto& o) {
		switch (o->type) {
		case MonsterTypes::Monster1: ((xx::Shared<Monster1>&)o)->Draw(); break;
		case MonsterTypes::Monster2: ((xx::Shared<Monster2>&)o)->Draw(); break;
		case MonsterTypes::Monster3: ((xx::Shared<Monster3>&)o)->Draw(); break;
		default: xx_assert(false);
		}
	});
	if (shooter) {
		shooter->Draw();
	}
	bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });
	bullets_shooter2.Foreach([&](auto& o) { o->Draw(); });
	effects_explosion.Foreach([&](auto& o) { o->Draw(); });
	effects_damageText.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw
}

