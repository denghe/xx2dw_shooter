#pragma once
#include "game_hero.h"

struct Afterimage : Sprite {
	constexpr static float cAlpha{ 0.8f };
	constexpr static float cAlphaDecrease{ cAlpha / 4 * 60 / gDesign.fps };

	float alpha{ cAlpha };

	void Init(Sprite const& tar) {
		mainLogic = MainLogic();
		pos = tar.pos;
		radians = tar.radians;
		frames = tar.frames;
		frameIndex = tar.frameIndex;
		body = tar.body;
		body.SetColorAf(alpha);
		body.SetColormulti(cAlpha);
	}

	xx::Task<> MainLogic() {
		while (alpha > 0) {
			alpha -= cAlphaDecrease;
			body.SetColorAf(alpha);
			body.SetColormulti(cAlpha);
			co_yield 0;
		}
	}
};
