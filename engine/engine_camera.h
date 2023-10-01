#pragma once
#include "engine_base.h"

struct Camera {
	// need set
	float scale{ 1 }, zoom{ 1 };
	XY original{};												// logic center position
	XY maxFrameSize{};											// for calculate safe area

	// calc fills
	float width_2{}, height_2{};								// logic size: window size * zoom
	float minX{}, maxX{}, minY{}, maxY{};						// 4 corner pos
	float safeMinX{}, safeMaxX{}, safeMinY{}, safeMaxY{};		// corner + max tex/frame size

	XX_FORCE_INLINE Camera& SetMaxFrameSize(XY const& maxFrameSize_) {
		maxFrameSize = maxFrameSize_;
		return *this;
	}

	XX_FORCE_INLINE Camera& SetOriginal(XY const& original_) {
		original = original_;
		return *this;
	}

	XX_FORCE_INLINE Camera& SetScale(float scale_) {
		scale = scale_;
		zoom = 1 / scale_;
		return *this;
	}

	XX_FORCE_INLINE Camera& SetZoom(float zoom_) {
		zoom = zoom_;
		scale = 1 / zoom_;
		return *this;
	}

	XX_FORCE_INLINE Camera& AddScale(float offset) {
		return SetScale(scale + offset);
	}

	XX_FORCE_INLINE Camera& IncreaseScale(float v, float maxScale) {
		scale += v;
		if (scale > maxScale) scale = maxScale;
		zoom = 1 / scale;
		return *this;
	}

	XX_FORCE_INLINE Camera& DecreaseScale(float v, float minScale) {
		scale -= v;
		if (scale < minScale) scale = minScale;
		zoom = 1 / scale;
		return *this;
	}

	// need set original & set scale or zoom
	void Calc() {
		width_2 = gEngineBase->windowWidth_2 * zoom;
		height_2 = gEngineBase->windowHeight_2 * zoom;

		minX = original.x - width_2;
		maxX = original.x + width_2;
		minY = original.y - height_2;
		maxY = original.y + height_2;

		safeMinX = minX - maxFrameSize.x;
		safeMaxX = maxX + maxFrameSize.x;
		safeMinY = minY - maxFrameSize.y;
		safeMaxY = maxY + maxFrameSize.y;
	}

	// need calc
	template<typename XY_t>
	XX_FORCE_INLINE bool InArea(XY_t const& pos) const {
		return pos.x >= safeMinX && pos.x <= safeMaxX
			&& pos.y >= safeMinY && pos.y <= safeMaxY;
	}

	// need calc
	XX_FORCE_INLINE XY ToGLPos(XY const& logicPos) {
		return (logicPos - original).MakeFlipY() * scale;
	}
	XX_FORCE_INLINE XY ToGLPos(Vec2<> const& logicPos) {
		return (logicPos.As<float>() - original).MakeFlipY() * scale;
	}
};
