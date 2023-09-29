#pragma once
#include "game_sprite.h"

struct Hero : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.15f };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	int heroId{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	float speed{ cSpeed };

	void Init(int heroId_, XY const& bornPos) {
		mainLogic = MainLogic();
		radius = cRadius;
		heroId = heroId_;
		pos = bornPos;
		SetDirection(MoveDirections::Down);
		frames = &gLooper.frames_heros[heroId];
		body.SetAnchor(cAnchor);
	}

	void SetDirection(MoveDirections d) {
		direction = d;
		frameIndexFrom = cFrameIndexRanges[(int)d];
		frameIndexTo = cFrameIndexRanges[(int)d + 1];
		frameIndex = frameIndexFrom;
	}

	xx::Task<> MainLogic() {
		while (true) {
			if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
				pos += r->second * speed;

				if (direction != r->first) {
					SetDirection(r->first);
				}
				FrameControl::Forward(frameIndex, cFrameInc * speed, frameIndexFrom, frameIndexTo);
			}
			co_yield 0;
		}
	}
};
