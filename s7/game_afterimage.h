#pragma once
#include "game_drawable_sprite.h"

struct Afterimage : Sprite {
	using ThisType = Afterimage;
	constexpr static float cAlpha{ 0.8f };
	constexpr static float cAlphaDecrease{ cAlpha / 4 * 60 / gDesign.fps };

	float alpha{ cAlpha };

	void Init(Sprite const& tar) {
		InitGetYDrawUpate<ThisType>();
		// copy propertiles
		pos = tar.pos;
		radians = tar.radians;
		frames = tar.frames;
		frameIndex = tar.frameIndex;
		body = tar.body;
		//body.SetColor({ 255, 127, 127, 255 });
		body.SetColorAf(alpha);
		body.SetColormulti(cAlpha);
	}

	xx::Task<> MainLogic{ MainLogic_() };
	xx::Task<> MainLogic_() {
		while (alpha > 0) {
			alpha -= cAlphaDecrease;
			body.SetColorAf(alpha);
			body.SetColormulti(cAlpha);
			co_yield 0;
		}
	}

};
