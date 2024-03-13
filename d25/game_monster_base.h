#pragma once
#include "looper.h"

struct MonsterBase : GridItemBase {
	static constexpr double cHP{ 100 };
	static constexpr float cRadius{ 16 };
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double hp{};					// fill by init
	float radius{};
	int trackIndex{};				// random by "safe range"
	float pointIndex{}, speed{}, radians{};
	// todo: more?
};
