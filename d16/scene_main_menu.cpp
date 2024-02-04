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

	audio.pauseFlag.Emplace();

	auto fn = "res/pistol.ogg"sv;
	fnData = std::move(gEngine->LoadFileData(fn).first);


	rootNode->MakeChildren<Button>()->Init(3, { -300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play 1.ogg 1 times", [&]() {
		for (size_t i = 0; i < 1; i++) {
			audio.Play(fnData, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play 1.ogg 5 times", [&]() {
		for (size_t i = 0; i < 3; i++) {
			audio.Play(fnData, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Loop Play 1.ogg 5 times", [&]() {
		for (size_t i = 0; i < 5; i++) {
			audio.Play(fnData, true);
		}
	});

	rootNode->MakeChildren<Button>()->Init(3, { -100, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Pause", [&]() {
		audio.Pause();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Stop", [&]() {
		audio.Stop();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 100, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Resume", [&]() {
		audio.Resume();
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
