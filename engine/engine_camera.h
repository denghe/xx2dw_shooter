#pragma once
#include "engine_base.h"

struct Camera {
	// need set
	XY original;
	float scale{ 1 }, zoom{ 1 };

	// calc fills
	float width_2{}, height_2{};	// window size * zoom
	float minX{}, maxX{}, minY{}, maxY{};
	float safeMinX{}, safeMaxX{}, safeMinY{}, safeMaxY{};

	XX_FORCE_INLINE Camera& SetOriginal(XY const& ori) {
		original = ori;
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
	void Calc(float safeX, float safeY) {
		width_2 = gEngine->windowWidth_2 * zoom;
		height_2 = gEngine->windowHeight_2 * zoom;

		minX = original.x - width_2;
		maxX = original.x + width_2;
		minY = original.y - height_2;
		maxY = original.y + height_2;

		safeMinX = minX - safeX;
		safeMaxX = maxX + safeX;
		safeMinY = minY - safeY;
		safeMaxY = maxY + safeY;
	}

	// need calc
	XX_FORCE_INLINE bool InArea(XY const& pos) const {
		return pos.x >= safeMinX && pos.x <= safeMaxX
			&& pos.y >= safeMinY && pos.y <= safeMaxY;
	}

	// need calc
	XX_FORCE_INLINE XY GetGLPos(XY const& logicPos) {
		return (logicPos - original).MakeFlipY() * scale;
	}
};
