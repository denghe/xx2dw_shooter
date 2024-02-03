#include <pch.h>
#include <all.h>

void SceneMainMenu::Update() {
	audio.Update();
}

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, { 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"XX SURVIVOR");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, { 0, 200 }, { 1,1 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }), gLooper.s9cfg_panel);

	auto btn = rootNode->MakeChildren<Button>();
	btn->Init(3, { 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"New Game", [&]() {
		gLooper.DelaySwitchTo<ScenePlay>();
	});

	auto fn = "res/1.ogg"sv;
	fnData = std::move(gEngine->LoadFileData(fn).first);

	rootNode->MakeChildren<Button>()->Init(3, { -100, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play 1.ogg 10000 times", [&]() {
		for (size_t i = 0; i < 10000; i++) {
			audio.Play(fnData);
		}
	});

}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
