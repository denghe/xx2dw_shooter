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
	ready = true;											// all tex ready

	root.Emplace()->FillTrans();
	root->MakeChildren<Button>()->Init(1, {0, -50}, 4, gLooper.frame_button, { 2,3,2,2 }, U"asd👻🎃fqwer");
	root->MakeChildren<Button>()->Init(1, {}, 4, gLooper.frame_button, { 2,3,2,2 }, U"ASADF");
	root->MakeChildren<Button>()->Init(1, {0, 50}, 4, gLooper.frame_button, { 2,3,2,2 }, U"zxcv123");

	//while (true) {
	//	for (float x = -100; x < 100; ++x) {
	//		root->position.x = x;
	//		root->FillTransRecursive();
	//		co_yield 0;
	//	}
	//	for (float x = 100; x > -100; --x) {
	//		root->position.x = x;
	//		root->FillTransRecursive();
	//		co_yield 0;
	//	}
	//}

	//xx::CoutN("root->trans = ", root->trans);
	//xx::CoutN("root->children[0]->trans = ", root->children[0]->trans);
	//auto p = root->children[0]->trans.MakeInvert()({});
	//xx::CoutN("p = ", p);
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
