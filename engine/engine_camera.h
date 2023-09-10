#pragma once
#include "engine_base.h"

struct Camera {
	XY original;
	float scale{ 1 }, zoom{ 1 };	// need set same time
	float width_2{}, height_2{};	// window size * zoom
	float minX{}, maxX{}, minY{}, maxY{};
	float safeMinX{}, safeMaxX{}, safeMinY{}, safeMaxY{};	// camera size + max frame size  for calc draw

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

	XX_FORCE_INLINE bool InArea(XY const& pos) const {
		return pos.x >= safeMinX && pos.x <= safeMaxX
			&& pos.y >= safeMinY && pos.y <= safeMaxY;
	}

	XX_FORCE_INLINE XY GetGLPos(XY const& logicPos) {
		return (logicPos - original).MakeFlipY() * scale;
	}
};
