#pragma once
#include "game_looper.h"

template<typename T> concept Has_Drawable_Update = requires(T t) { { t.Update() } -> std::same_as<bool>; };

struct Drawable {
	virtual ~Drawable() {}

	XY pos{};

	xx::Task<> mainLogic;

	virtual bool Update() {
		xx_assert(false);
		return false;
	}

	// default impl
	virtual float GetY(Camera const& camera) {
		if (camera.InArea(pos)) {
			return pos.y;
		}
		return std::numeric_limits<float>::quiet_NaN();
	}

	virtual void Draw() {
		xx_assert(false);
	}
};


template<typename T>
struct Removeable {
	xx::ListDoubleLink<xx::Shared<T>, int32_t, uint32_t>* container{};
	xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t> indexAndVersionByContainer;

	template<typename U = T>
	static xx::Shared<U> CreateTo(xx::ListDoubleLink<xx::Shared<T>, int32_t, uint32_t>& container) {
		auto rtv = xx::Make<U>();
		container.Emplace(rtv);
		rtv->container = &container;
		rtv->indexAndVersionByContainer = container.Tail();
		return rtv;
	}

	void RemoveFromOwner() {
		auto c = container;
		xx_assert(c);
		auto iv = indexAndVersionByContainer;
		assert(iv.index != -1);
		container = {};
		indexAndVersionByContainer = { -1, 0 };
		c->Remove(iv);
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
	XY scale{ 1, 1 };
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

	virtual void Draw() override {
		body.SetScale(scale * XY{ flipX ? -gLooper.camera.scale : gLooper.camera.scale, gLooper.camera.scale })
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.SetRotate(radians)
			.SetFrame((*frames)[frameIndex])
			.Draw();
	}

};
