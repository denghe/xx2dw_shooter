#pragma once
#include "looper.h"

struct MonsterBase : GridItemBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
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

struct TowerBase : GridItemBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cRadius{ 16 };
	static constexpr float cFireDelay{ 1.f / gDesign.fps };
	// todo: more?
};

struct BulletBase : GridItemBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr double cDamage{ 100 };
	static constexpr float cRadius{ 16 };
	static constexpr float cSpeed{ 600.f / gDesign.fps };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double damage{};					// fill by init
	float radius{};
	float speed{}, radians{};
	int32_t deathFrameNumber{};
	// todo: more?
};
