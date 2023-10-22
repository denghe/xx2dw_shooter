#include "pch.h"

xx::Task<> GameLooper::MainTask() {
#ifdef __EMSCRIPTEN__
	s9cfg.frame = Frame::Create(co_await AsyncLoadTextureFromUrl("res/button.png"));
#else
	s9cfg.frame = LoadFrame("res/button.png");
#endif

#if 1
	root.Emplace()->scale = { 0.45f, 0.45f };
	root->FillTrans();
	for (size_t i = 0; i < 10000; i++) {
		XY pos{ rnd.Next<float>(-width, width), rnd.Next<float>(-height, height) };
		auto cfg = s9cfg;
		(uint32_t&)s9cfg.color = rnd.Next<uint32_t>();
		s9cfg.color.a = 255;
		auto txt = xx::StringU8ToU32(rnd.NextWord());
		root->MakeChildren<Button>()->Init(1, pos, { 0.5, 0.5 }, cfg, txt);
	}
#else
	root.Emplace()->scale = { 3, 3 };
	root->FillTrans();
	root->MakeChildren<Button>()->Init(1, { 0, -50 }, {0.5, 0.5}, buttonTextureScale, frame_button, buttonUvRect, { 0x5f, 0x15, 0xd9, 0xff }, U"asd👻🎃fqwer");
	root->MakeChildren<Button>()->Init(1, {}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0x2b, 0x39, 0xfb, 0xff }, U"ASADF");
	root->MakeChildren<Button>()->Init(1, {0, 50}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0xe7, 0x8d, 0x00, 0xff }, U"zxcv123");
#endif

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
#ifdef __EMSCRIPTEN__
	if (!s9cfg.frame) return;
#endif
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	LineStrip().FillCirclePoints({ 0, 0 }, 2, {}, 8).Draw();
	LineStrip().FillCirclePoints({ 0, 120 }, 2, {}, 8).Draw();
}
