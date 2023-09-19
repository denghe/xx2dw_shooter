#pragma once
#include "game_looper.h"

struct Sprite {
	constexpr static float cIdleScaleYFrom{ 0.9f };
	constexpr static float cIdleScaleYTo{ 1.f };
	constexpr static float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	mutable Quad body;
	XY pos, scale{ 1, 1 };
	float radius{}, radians{};
	float frameIndex{};
	bool flipX{};

	std::vector<xx::Shared<Frame>>const* frames{};
	xx::Task<> mainLogic;

	xx::Task<> Idle { Idle_() };
	xx::Task<> Idle_() {
		while (true) {
			for (scale.y = cIdleScaleYTo; scale.y > cIdleScaleYFrom; scale.y -= cIdleScaleYStep) co_yield 0;
			for (scale.y = cIdleScaleYFrom; scale.y < cIdleScaleYTo; scale.y += cIdleScaleYStep) co_yield 0;
		}
	}

	// todo: GetYSprite

	void Draw() const {
		body.SetScale(scale * XY{ flipX ? -gLooper.camera.scale : gLooper.camera.scale, gLooper.camera.scale })
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.SetRotate(radians)
			.SetFrame((*frames)[frameIndex])
			.Draw();
	}

	XX_FORCE_INLINE void ForwardFrame(float inc, float from, float to) {
		frameIndex += inc;
		if (frameIndex >= to) {
			frameIndex = from + (frameIndex - to);
		}
	}
	XX_FORCE_INLINE void ForwardFrame(float inc, float to) {
		ForwardFrame(inc, 0, to);
	}

	XX_FORCE_INLINE	void BackwardFrame(float inc, float from, float to) {
		frameIndex -= inc;
		if (frameIndex <= from) {
			frameIndex = to - (from - frameIndex);
		}
	}
	XX_FORCE_INLINE	void BackwardFrame(float inc, float to) {
		BackwardFrame(inc, 0, to);
	}

	bool StepRadians(float r, float frameMaxChangeRadian) {
		bool done{};
		//r = -r;
		auto d = r - radians;
		if (d * d > M_PI * M_PI) {
			if (r < radians) {
				r += M_PI * 2;
			} else {
				r -= M_PI * 2;
			}
		}
		if (r > radians) {
			if (r - radians <= frameMaxChangeRadian) {
				radians = r;
				done = true;
			} else {
				radians += frameMaxChangeRadian;
			}
			if (radians > M_PI) {
				radians -= M_PI * 2;
			}
		} else {
			if (radians - r <= frameMaxChangeRadian) {
				radians = r;
				done = true;
			} else {
				radians -= frameMaxChangeRadian;
			}
			if (radians < -M_PI) {
				radians += M_PI * 2;
			}
		}
		return done;
	}

	// todo: HitCheck
};
