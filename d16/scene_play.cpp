#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// todo: init hero
	// 
	//auto wh_2 = gLooper.windowSize_2;
	//for (size_t i = 0; i < 1000; i++) {
	//	auto&& h = heroes.Emplace().Emplace();
	//	h->pos = { gLooper.rnd.Next<float>(-wh_2.x + 50, wh_2.x - 50)
	//		, gLooper.rnd.Next<float>(-wh_2.y + 10, wh_2.y - 100) };
	//	h->Init(gLooper.rnd.Next<float>(1,3), gLooper.rnd.Next<int>(0, 100), 100);
	//}

	Config::TestGun1();
}

void ScenePlay::Draw() {
	auto& eb = EngineBase1::Instance();

	//hero->Draw();
	//hero->DrawHP();

	gLooper.DrawNode(rootNode);
};
