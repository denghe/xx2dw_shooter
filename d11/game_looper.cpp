#include "pch.h"

xx::Task<> GameLooper::MainTask() {
#ifdef __EMSCRIPTEN__
	frame_button = co_await AsyncLoadFrameFromUrl("res/button.png");
#else
	frame_button = LoadFrame("res/button.png");
#endif

	constexpr XY anchor{ 0.5, 0.5 };
	Scale9SpriteConfig cfg{ frame_button, { 4,4 }, { 2,3,2,2 }, { 0xe7, 0x8d, 0x00, 0xff } };
	root.Emplace()->FillTrans();
	root->MakeChildren<Button>()->Init(0, { -50, 0 }, anchor, cfg, U"A");
	root->MakeChildren<Button>()->Init(0, { 0, 0 }, anchor, cfg, U"S");
	root->MakeChildren<Button>()->Init(0, { 50, 0 }, anchor, cfg, U"D");
	root->MakeChildren<Button>()->Init(0, { 150, 0 }, anchor, cfg, U"J");
	kct.Emplace();
	ready = true;
	co_return;
}

void GameLooper::Update() {
	if (!ready) return;
	root->children[0]->children[1].Cast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::A) ? 1.5f : 1;
	root->children[1]->children[1].Cast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::S) ? 1.5f : 1;
	root->children[2]->children[1].Cast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::D) ? 1.5f : 1;
	root->children[3]->children[1].Cast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::J) ? 1.5f : 1;
	kct->Update();
}

void GameLooper::Draw() {
	if (!ready) return;
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	if (!kct->msg.empty()) {
		gLooper.ctcDefault.Draw({ 0, -100 }, kct->msg);
	}
}
