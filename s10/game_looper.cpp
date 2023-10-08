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

	root.Emplace()->scale = { 0.45, 0.45 };
	root->FillTrans();
	//root->MakeChildren<Button>()->Init(1, { 0, -50 }, {0.5, 0.5}, buttonTextureScale, frame_button, buttonUvRect, { 0x5f, 0x15, 0xd9, 0xff }, U"asd👻🎃fqwer");
	//root->MakeChildren<Button>()->Init(1, {}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0x2b, 0x39, 0xfb, 0xff }, U"ASADF");
	//root->MakeChildren<Button>()->Init(1, {0, 50}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0xe7, 0x8d, 0x00, 0xff }, U"zxcv123");

	//auto lbl = root->MakeChildren<Label>();
	//lbl->Init(5, { 0, 0 }, { 1, 1 }, { 0.5, 0.5 }, RGBA8_White, U"this is label");
	//root->MakeChildren<Scale9Sprite>()->Init(4, { 0, 0 }, { 0.5, 0.5 }, 4, lbl->size, frame_button, buttonUvRect, {0x2b, 0x39, 0xfb, 0xff});

	//rnd.NextWord()
	for (size_t i = 0; i < 10000; i++) {
		XY pos{ rnd.Next<float>(-width, width), rnd.Next<float>(-height, height) };
		RGBA8 color;
		(uint32_t&)color = rnd.Next<uint32_t>();
		color.a = 255;
		auto txt = xx::StringU8ToU32(rnd.NextWord());
		root->MakeChildren<Button>()->Init(1, pos, {0.5, 0.5}, buttonTextureScale, frame_button, buttonUvRect, color, txt);
	}
	

	//while (true) {
	//	for (float x = -100; x < 100; ++x) {
	//		root->position.x = x;
	//		root->position.y = x;
	//		root->FillTransRecursive();
	//		co_yield 0;
	//	}
	//	for (float x = 100; x > -100; --x) {
	//		root->position.x = x;
	//		root->position.y = x;
	//		root->FillTransRecursive();
	//		co_yield 0;
	//	}
	//}
	co_yield 0;
}

void GameLooper::Update() {
	if (!ready) return;
}

void GameLooper::Draw() {
	if (!ready) return;
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	LineStrip().FillCirclePoints({0, 0}, 2, {}, 8).Draw();
	LineStrip().FillCirclePoints({0, 120}, 2, {}, 8).Draw();
}
