#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include "xx_blocklink.h"

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto basePos = rootNode->size / 2;

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"astar tests");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, basePos + XY{ 0, 200 }, { 5, 5 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }) / 5, gLooper.s9cfg);

	rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
		gLooper.DelaySwitchTo<Test1::Scene>();
	}, 3);


	//Rnd rnd;
	//int64_t counter{};
	//xx::BlockLink<xx::Task<>> tasks;
	//auto secs = xx::NowEpochSeconds();
	//for (size_t i = 0; i < 200000; i++) {
	//	tasks.Emplace(([](Rnd& rnd, int64_t& counter)->xx::Task<> {
	//		for (int i = 0, e = rnd.Next(100, 1000);
	//			i < e;
	//			i++) {
	//			++counter;
	//			co_yield 0;
	//		}
	//		})(rnd, counter));
	//}
	//do {
	//	//tasks.Foreach([](auto& o)->xx::ForeachResult {
	//	tasks.ForeachLink([](auto& o)->xx::ForeachResult {
	//		return o.Resume() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
	//	});
	//} while (tasks.Count());

	//xx::CoutN(xx::NowEpochSeconds(secs));
	//xx::CoutN(counter);
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
