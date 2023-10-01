#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();											// font init
	{
		auto tex = co_await AsyncLoadTextureFromUrl("res/button.png");
		xx_assert(tex);
		frame_button = Frame::Create(tex);
	}
	ready = true;											// all tex ready

	root.Emplace()->children.Emplace().Emplace<Button>()->Init(
		1, {}, 4, gLooper.frame_button, { 2,3,2,2 }, "asdfqwer");

	//while (true) {
	//	for (float x = -100; x < 100; ++x) {
	//		//printf("x = %f\n", x);
	//		root->position.x = x;
	//		root->dirty = true;
	//		co_yield 0;
	//	}
	//	for (float x = 100; x > -100; --x) {
	//		//printf("x = %f\n", x); 
	//		root->position.x = x;
	//		root->dirty = true;
	//		co_yield 0;
	//	}
	//}
}

void GameLooper::Update() {
	fv.Update();
	if (!ready) return;										// todo: show loading ?
}

void GameLooper::Draw() {
	if (ready) {

		PredrawAndFillTo(tmpZNodes, root);
		OrderByZDrawAndClear(tmpZNodes);
	}
	fv.Draw(ctc72);											// show fps
}
