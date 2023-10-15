#pragma once
#include "game_drawable_sprite.h"

struct Blood : Sprite {
	constexpr static char const* cResPrefix{ "blood_4_" };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };

	void Init(XY const& pos_) {
		mainLogic = MainLogic_();
		pos = pos_;
		radians = gEngine->rnd.Next<float>((float)(M_PI * 2));
		frames = &gLooper.frames_blood_4;
		frameIndex = {};
	}

	xx::Task<> MainLogic_() {
		float frameMaxIndex = (float)frames->size();
		do {
			co_yield 0;
			frameIndex += cFrameInc;
		} while (frameIndex < frameMaxIndex);
	}

};
