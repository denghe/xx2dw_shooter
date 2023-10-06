#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;

xx::Task<> GameLooper::MainTask() {
	{
		auto tex = co_await AsyncLoadTextureFromUrl("res/button.png");
		xx_assert(tex);
		frame_button = Frame::Create(tex);
	}
	ready = true;

	root.Emplace()->FillTrans();
	root->MakeChildren<Button>()->Init(1, {0, -50}, buttonTextureScale, frame_button, buttonUvRect, { 0x5f, 0x15, 0xd9, 0xff }, U"asd👻🎃fqwer");
	root->MakeChildren<Button>()->Init(1, {}, buttonTextureScale, frame_button, buttonUvRect, { 0x2b, 0x39, 0xfb, 0xff }, U"ASADF");
	root->MakeChildren<Button>()->Init(1, {0, 50}, buttonTextureScale, frame_button, buttonUvRect, { 0xe7, 0x8d, 0x00, 0xff }, U"zxcv123");

	while (true) {
		for (float x = -100; x < 100; ++x) {
			root->position.x = x;
			root->FillTransRecursive();
			co_yield 0;
		}
		for (float x = 100; x > -100; --x) {
			root->position.x = x;
			root->FillTransRecursive();
			co_yield 0;
		}
	}
}

void GameLooper::Update() {
	if (!ready) return;
}

void GameLooper::Draw() {
	if (!ready) return;
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	//LineStrip().FillCirclePoints({}, 2, {}, 8).Draw();
}
