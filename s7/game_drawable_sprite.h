#pragma once
#include "game_looper.h"

template<typename T> concept Has_Drawable_Update = requires(T t) { { t.Update() } -> std::same_as<bool>; };

struct Drawable {
	XY pos{};

	// for hardware draw
	// get pos.y for order by Y
	// return std::numeric_limits<float>::quiet_NaN(): do not need draw
	float (*getY)(void* derived, Camera const& camera) = {};

	// for hardware draw
	void (*draw)(void* derived) = {};

	// for logic update
	bool (*update)(void* derived) = {};

	// classic init for example

	template<typename Derived>
	void InitGetYDrawUpate() {
		getY = [](void* self, Camera const& camera) { return ((Derived*)self)->GetY(camera);  };
		draw = [](void* self) { ((Derived*)self)->Draw(); };
		if constexpr (Has_Drawable_Update<Derived>) {
			update = [](void* self)->bool { return ((Derived*)self)->Update(); };
		} else {
			update = [](void* self)->bool { return ((Derived*)self)->MainLogic.Resume(); };
		}
	}

	// default impl
	float GetY(Camera const& camera) {
		if (camera.InArea(pos)) {
			return pos.y;
		}
		return std::numeric_limits<float>::quiet_NaN();
	}
};


struct Sprite : Drawable {
	constexpr static float cIdleScaleYFrom{ 0.9f };
	constexpr static float cIdleScaleYTo{ 1.f };
	constexpr static float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	constexpr static float cIdleRotateRadiansFrom{ -0.07f };
	constexpr static float cIdleRotateRadiansTo{ 0.07f };
	constexpr static float cIdleRotateRadiansStep{ 2.f / gDesign.fps };

	mutable Quad body;
	XY pos, scale{ 1, 1 };
	float radius{}, radians{};
	float frameIndex{};
	bool flipX{};

	std::vector<xx::Shared<Frame>>const* frames{};

	xx::Task<> idle;		// need init

	xx::Task<> Idle_ScaleY() {
		while (true) {
			for (scale.y = cIdleScaleYTo; scale.y > cIdleScaleYFrom; scale.y -= cIdleScaleYStep) co_yield 0;
			for (scale.y = cIdleScaleYFrom; scale.y < cIdleScaleYTo; scale.y += cIdleScaleYStep) co_yield 0;
		}
	}

	xx::Task<> Idle_Rotate() {
		while (true) {
			for (radians = cIdleRotateRadiansFrom; radians < cIdleRotateRadiansTo; radians += cIdleRotateRadiansStep) co_yield 0;
			for (radians = cIdleRotateRadiansTo; radians >= cIdleRotateRadiansFrom; radians -= cIdleRotateRadiansStep) co_yield 0;
		}
	}

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
