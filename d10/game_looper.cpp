#include "pch.h"

xx::Task<> GameLooper::MainTask() {
#ifdef __EMSCRIPTEN__
	s9cfg.frame = Frame::Create(co_await AsyncLoadTextureFromUrl("res/button.png"));
#else
	s9cfg.frame = LoadFrame("res/button.png");
#endif

#if 0
	root.Emplace()->Init();
	//root->scale = { 0.45f, 0.45f };
	root->FillTrans();
	for (size_t i = 0; i < 10000; i++) {
		XY pos{ rnd.Next<float>(100, root->size.x - 100), rnd.Next<float>(50, root->size.y - 50) };
		auto cfg = s9cfg;
		(uint32_t&)s9cfg.color = rnd.Next<uint32_t>();
		s9cfg.color.a = 255;
		auto txt = xx::StringU8ToU32(rnd.NextWord());
		root->MakeChildren<Button>()->Init(1, pos, { 0.5, 0.5 }, cfg, txt);
	}
#else
	root.Emplace()->Init();
	root->scale = { 1.5f, 1.5f };
	root->FillTrans();
	auto basePos = root->size / 2;
	root->MakeChildren<Button>()->Init(1, basePos + XY{ 0, -50 }, {0.5, 0.5}, s9cfg, U"asd👻🎃fqwer");
	root->MakeChildren<Button>()->Init(1, basePos + XY{}, { 0.5, 0.5 }, s9cfg, U"ASADF");
	root->MakeChildren<Button>()->Init(1, basePos + XY{0, 50}, { 0.5, 0.5 }, s9cfg, U"zxcv123");
#endif

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
#ifdef __EMSCRIPTEN__
	if (!s9cfg.frame) return;
#endif
	DrawNode(root);
	//LineStrip().FillCirclePoints({ 0, 0 }, 2, {}, 8).Draw();
	//LineStrip().FillCirclePoints({ 0, 120 }, 2, {}, 8).Draw();
}
