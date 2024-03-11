#pragma once
#include <engine_prims.h>

struct Cfg {
	static constexpr float maxItemSize{ 32 };

	static constexpr int physCellSize{ 32 };	// need >= max item size
	static constexpr int physNumRows{ 512 };
	static constexpr int physNumCols{ 512 };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
	static constexpr XY mapCenterPos{ mapSize_2 };
	static constexpr XY mapSafeMinPos{ maxItemSize, maxItemSize };
	static constexpr XY mapSafeMaxPos{ mapSize.x - maxItemSize, mapSize.y - maxItemSize };

	static constexpr float mouseHitRange{ 50.f };
};
inline Cfg gCfg;
