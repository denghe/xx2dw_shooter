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

/*
todo: polygon circle
docs here:

https://www.sevenson.com.au/programming/sat/
https://github.com/sevdanski/SAT_JS/blob/main/src/js/sat.js
https://www.codeproject.com/Articles/15573/2D-Polygon-Collision-Detection
http://www.jeffreythompson.org/collision-detection/table_of_contents.php

http://www.jeffreythompson.org/collision-detection/line-circle.php
https://github.com/williamfiset/Algorithms/blob/master/src/main/java/com/williamfiset/algorithms/geometry/LineSegmentCircleIntersection.js
https://stackoverflow.com/questions/4226356/circle-to-circle-segment-collision

*/

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();											// font init
	{
		auto tex = co_await AsyncLoadTextureFromUrl("res/button.png");
		xx_assert(tex);
		frame_button = Frame::Create(tex);
	}
	ready = true;											// all tex ready

	ls.FillCirclePoints({}, 100);
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;										// todo: show loading ?

}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		ls.Draw();
	}
	fv.Draw(ctc72);											// show fps
}
