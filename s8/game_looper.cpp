﻿#include "pch.h"

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

}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;										// todo: show loading ?

	buttons.Foreach([&](auto& o) {
		return o->mainLogic.Resume();
	});
}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		buttons.Foreach([&](auto& o) {
			if (gLooper.camera.InArea(o->pos)) {
				o->Draw();
			}
		});
	}
	fv.Draw(ctc72);											// show fps
}

/*
todo: polygon circle 
docs here:

https://www.sevenson.com.au/programming/sat/
https://github.com/sevdanski/SAT_JS/blob/main/src/js/sat.js
https://www.codeproject.com/Articles/15573/2D-Polygon-Collision-Detection
http://www.jeffreythompson.org/collision-detection/table_of_contents.php

*/

void Button::Init(XY const& pos_, XY const& size_, std::string_view const& txt_) {
	assert(size.x > 6 && size.y > 8);
	pos = pos_;
	size = size_;
	txt = txt_;
}

xx::Task<> Button::MainLogic() {
	while (true) {
		// todo: scan mouse state & calc area ?

		co_yield 0;
	}
}

void Button::Draw() {

}
