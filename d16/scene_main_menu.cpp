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

	rootNode->MakeChildren<Button>()->Init(3, { 0, 100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"New Game", [&]() {
		gLooper.DelaySwitchTo<ScenePlay>();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, 50 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"New Game 2", [&]() {
		gLooper.DelaySwitchTo<ScenePlay2>();
	});

	audio.pauseFlag.Emplace();

	
#ifdef __EMSCRIPTEN__
	tasks.Add([&]()->xx::Task<> {
		datas.Emplace(std::move(*co_await gLooper.AsyncDownloadFromUrl("res/button1.ogg")));
		datas.Emplace(std::move(*co_await gLooper.AsyncDownloadFromUrl("res/gun1.ogg")));
		datas.Emplace(std::move(*co_await gLooper.AsyncDownloadFromUrl("res/gun2.ogg")));
		datas.Emplace(std::move(*co_await gLooper.AsyncDownloadFromUrl("res/gun3.ogg")));
	});
#else
	datas.Emplace(std::move(gEngine->LoadFileData("res/button1.ogg"sv).first));
	datas.Emplace(std::move(gEngine->LoadFileData("res/gun1.ogg"sv).first));
	datas.Emplace(std::move(gEngine->LoadFileData("res/gun2.ogg"sv).first));
	datas.Emplace(std::move(gEngine->LoadFileData("res/gun3.ogg"sv).first));
#endif


	rootNode->MakeChildren<Button>()->Init(3, { -100, -50 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Pause", [&]() {
		audio.Pause();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -50 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Stop", [&]() {
		audio.Stop();
	});
	rootNode->MakeChildren<Button>()->Init(3, { 100, -50 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Resume", [&]() {
		audio.Resume();
	});

	rootNode->MakeChildren<Button>()->Init(3, { -300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play button1.ogg 1 times", [&]() {
		audio.Play(datas[0]);
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Play button1.ogg 5 times", [&]() {
		for (size_t i = 0; i < 5; i++) {
			audio.Play(datas[0]);
		}
	});
	rootNode->MakeChildren<Button>()->Init(3, { 300, -100 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Loop Play button1.ogg", [&]() {
		audio.Play(datas[0], true);
	});

	rootNode->MakeChildren<Button>()->Init(3, { -300, -150 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"gun1.ogg", [&]() {
		audio.Play(datas[1]);
	});
	rootNode->MakeChildren<Button>()->Init(3, { 0, -150 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"gun2.ogg", [&]() {
		audio.Play(datas[2]);
	});
	rootNode->MakeChildren<Button>()->Init(3, { 300, -150 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"gun3.ogg", [&]() {
		audio.Play(datas[3]);
	});

}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
