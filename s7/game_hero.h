#pragma once
#include "game_sprite.h"

struct Hero : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	float frameIndexFrom{}, frameIndexTo{};
	float speed{ cSpeed };

	void Init(int heroId_, XY const& bornPos) {
		MainLogic = MainLogic_();
		radius = cRadius;
		pos = bornPos;
		frames = &gLooper.frames_pumpkin;
		body.SetAnchor(cAnchor);
	}

	xx::Task<> MainLogic_() {
		while (true) {

			// keyboard move control
			if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
				pos += r->second * speed;
				if (flipX && ((int)r->first & (int)MoveDirections::Right)) {
					flipX = false;
				} else if ((int)r->first & (int)MoveDirections::Left) {
					flipX = true;
				}
				ForwardFrame(cFrameInc * speed, cFrameMaxIndex);
			} else {
				Idle();
			}

			co_yield 0;
		}
	}

};
