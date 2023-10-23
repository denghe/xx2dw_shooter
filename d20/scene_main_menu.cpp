#include <pch.h>
#include <all.h>

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, { 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"dungeon portal survivor");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, { 0, 200 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }), gLooper.s9cfg_panel);

	auto btn = rootNode->MakeChildren<Button>();
	btn->Init(3, { 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"New Game", [&]() { 
		gLooper.DelaySwitchTo<ScenePlay>();
	});

	// debug: show hp bar
	q.SetScale(1).SetPosition({ 0, -200 });
	auto& fs = gLooper.hpBarCache->fs;
	tasks.Add([&]()->xx::Task<> {
		for (auto& f : fs) {
			q.SetFrame(f);
			co_yield 0;
		}
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);

	if (q) q.Draw();
};
