﻿#pragma once
#include "game_drawable_sprite.h"

struct Afterimage : Sprite {
	constexpr static float cAlpha{ 0.8f };
	constexpr static float cAlphaDecrease{ cAlpha / 4 * 60 / gDesign.fps };

	float alpha{ cAlpha };

	void Init(Sprite const& tar) {
		mainLogic = MainLogic_();
		// copy propertiles
		pos = tar.pos;
		radians = tar.radians;
		frames = tar.frames;
		frameIndex = tar.frameIndex;
		body = tar.body;
		//body.SetColor({ 255, 127, 127, 255 });
		body.SetColorAf(alpha);
	}

	xx::Task<> MainLogic_() {
		while (alpha > 0) {
			alpha -= cAlphaDecrease;
			body.SetColorAf(alpha);
			co_yield 0;
		}
	}

};
