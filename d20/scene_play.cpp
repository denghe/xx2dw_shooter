#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, { 0, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Exit Game", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	for (size_t i = 0; i < 10000; i++) {
		auto&& h = heroes.Emplace().Emplace();
		h->pos = { gLooper.rnd.Next<float>(-gDesign.width_2 + 50, gDesign.width_2 - 50)
			, gLooper.rnd.Next<float>(-gDesign.height_2 + 10, gDesign.height_2 - 100) };
		h->Init(gLooper.rnd.Next<float>(1,3), gLooper.rnd.Next<float>(0, 100), 100);
	}

	//tasks.Add([this]()->xx::Task<> {
	//	while (true) {
	//		for (; hero.hp < 100; ++hero.hp) {
	//			co_yield 0;
	//		}
	//		for (; hero.hp > 0; --hero.hp) {
	//			co_yield 0;
	//		}
	//	}
	//});
}

void ScenePlay::Draw() {
	for (auto& h : heroes) {
		h->Draw();
	}
	for (auto& h : heroes) {
		h->DrawHP();
	}
	gLooper.DrawNode(rootNode);
};
