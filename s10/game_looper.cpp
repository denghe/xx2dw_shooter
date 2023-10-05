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

	mouseEventHandlers.Init(128, 128, gDesign.width * 2, gDesign.height * 2);
}

EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
	mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
	assert(!mouseEventHandler);
	mouseEventHandlers.ForeachPoint(mouseEventHandlers.max_2 + mouse.pos, [&](auto o) {
		tmpZNodes.Emplace(o->z, o);
	});
	std::sort(tmpZNodes.buf, tmpZNodes.buf + tmpZNodes.len, ZNode::GreaterThanComparer);	// event big z first
	for (auto& zn : tmpZNodes) {
		((MouseEventHandlerNode*)zn.n)->OnMouseDown();
		if (mouseEventHandler) break;
	}
	tmpZNodes.Clear();
	return EM_TRUE;
}

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
	mouse.pos = { (float)e.targetX - this->windowWidth_2, this->windowHeight - (float)e.targetY - this->windowHeight_2 };
	if (mouseEventHandler) {
		mouseEventHandler->OnMouseMove();
	} else {
		mouseEventHandlers.ForeachPoint(mouseEventHandlers.max_2 + mouse.pos, [&](auto o) {
			tmpZNodes.Emplace(o->z, o);
		});
		std::sort(tmpZNodes.buf, tmpZNodes.buf + tmpZNodes.len, ZNode::GreaterThanComparer);	// event big z first
		for (auto& zn : tmpZNodes) {
			((MouseEventHandlerNode*)zn.n)->OnMouseMove();
			if (mouseEventHandler) break;
		}
		tmpZNodes.Clear();
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnMouseUp(EmscriptenMouseEvent const& e) {
	mouse.btnStates[e.button] = false;
	if (mouseEventHandler) {
		mouseEventHandler->OnMouseUp();
		mouseEventHandler = {};
	}
	return EM_TRUE;
}


xx::Task<> GameLooper::MainTask() {
	{
		auto tex = co_await AsyncLoadTextureFromUrl("res/button.png");
		xx_assert(tex);
		frame_button = Frame::Create(tex);
	}
	ready = true;											// all tex ready

	root.Emplace();
	root->children.Emplace().Emplace<Button>()->Init(1, {0, -50}, 4, gLooper.frame_button, { 2,3,2,2 }, U"asd👻🎃fqwer");
	root->children.Emplace().Emplace<Button>()->Init(1, {}, 4, gLooper.frame_button, { 2,3,2,2 }, U"qwer");
	root->children.Emplace().Emplace<Button>()->Init(1, {0, 50}, 4, gLooper.frame_button, { 2,3,2,2 }, U"zxcv123");

	//while (true) {
	//	for (float x = -100; x < 100; ++x) {
	//		//printf("x = %f\n", x);
	//		root->SetPositionX(x);
	//		co_yield 0;
	//	}
	//	for (float x = 100; x > -100; --x) {
	//		//printf("x = %f\n", x); 
	//		root->SetPositionX(x);
	//		co_yield 0;
	//	}
	//}
}

void GameLooper::Update() {
	if (!ready) return;
}

void GameLooper::Draw() {
	if (!ready) return;
	root->Update();
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	LineStrip().FillCirclePoints({}, 2, {}, 8).Draw();
}
