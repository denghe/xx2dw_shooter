#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

xx::Task<> GameLooper::MainTask() {
	{
		auto tex = co_await AsyncLoadTextureFromUrl("res/button.png");
		xx_assert(tex);
		frame_button = Frame::Create(tex);
	}
	static constexpr XY anchor{ 0.5, 0.5 };
	static constexpr float textureScale = 4;
	static constexpr UVRect uvr{ 2,3,2,2 };
	static constexpr RGBA8 color{ 0xe7, 0x8d, 0x00, 0xff };
	root.Emplace()->FillTrans();
	root->MakeChildren<Button>()->Init(0, { -50, 0 }, anchor, textureScale, frame_button, uvr, color, U"A");
	root->MakeChildren<Button>()->Init(0, { 0, 0 }, anchor, textureScale, frame_button, uvr, color, U"S");
	root->MakeChildren<Button>()->Init(0, { 50, 0 }, anchor, textureScale, frame_button, uvr, color, U"D");
	root->MakeChildren<Button>()->Init(0, { 150, 0 }, anchor, textureScale, frame_button, uvr, color, U"J");
	kct.Emplace();
	ready = true;
	co_return;
}

void GameLooper::Update() {
	if (!ready) return;
	root->children[0]->children[1].ReinterpretCast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::A) ? 1.5 : 1;
	root->children[1]->children[1].ReinterpretCast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::S) ? 1.5 : 1;
	root->children[2]->children[1].ReinterpretCast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::D) ? 1.5 : 1;
	root->children[3]->children[1].ReinterpretCast<Scale9Sprite>()->colorplus = KeyDown(KeyboardKeys::J) ? 1.5 : 1;
	kct->Update();
}

void GameLooper::Draw() {
	if (!ready) return;
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
}
