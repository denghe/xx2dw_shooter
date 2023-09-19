#pragma once
#include "game_sprite.h"

struct Hero : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.15f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	float frameIndexFrom{}, frameIndexTo{};
	float speed{ cSpeed };

	void Init(int heroId_, XY const& bornPos) {
		mainLogic = MainLogic();
		radius = cRadius;
		pos = bornPos;
		frames = &gLooper.frames_pumpkin;
		body.SetAnchor(cAnchor);
	}

	xx::Task<> MainLogic() {
		while (true) {
			if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
				pos += r->second * speed;
				flipX = ((int)r->first & (int)MoveDirections::Left) > 0;
				ForwardFrame(cFrameInc * speed, cFrameMaxIndex);
			}
			co_yield 0;
		}
	}
};
