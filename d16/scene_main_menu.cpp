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

	
#ifdef __EMSCRIPTEN__
	tasks.Add([&]()->xx::Task<> {
		data_1 = std::move(*co_await gLooper.AsyncDownloadFromUrl("res/1.ogg"));
		data_pistol = std::move(*co_await gLooper.AsyncDownloadFromUrl("res/pistol.ogg"));
	});
#else
	data_1 = std::move(gEngine->LoadFileData("res/1.ogg"sv).first);
	data_pistol = std::move(gEngine->LoadFileData("res/pistol.ogg"sv).first);
#endif


	rootNode->MakeChildren<Button>()->Init(3, { -300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play 1.ogg 1 times", [&]() {
		for (size_t i = 0; i < 1; i++) {
			audio.Play(data_1, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play 1.ogg 5 times", [&]() {
		for (size_t i = 0; i < 3; i++) {
			audio.Play(data_1, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Loop Play 1.ogg 5 times", [&]() {
		for (size_t i = 0; i < 5; i++) {
			audio.Play(data_1, true);
		}
	});

	rootNode->MakeChildren<Button>()->Init(3, { -300, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play pistol.ogg 1 times", [&]() {
		for (size_t i = 0; i < 1; i++) {
			audio.Play(data_pistol, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play pistol.ogg 5 times", [&]() {
		for (size_t i = 0; i < 3; i++) {
			audio.Play(data_pistol, false);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 300, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Loop Play pistol.ogg 5 times", [&]() {
		for (size_t i = 0; i < 5; i++) {
			audio.Play(data_pistol, true);
		}
	});

	rootNode->MakeChildren<Button>()->Init(3, { -100, -300 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Pause", [&]() {
		audio.Pause();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -300 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Stop", [&]() {
		audio.Stop();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 100, -300 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Resume", [&]() {
		audio.Resume();
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
