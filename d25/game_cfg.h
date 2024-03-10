#pragma once
#include <engine_prims.h>

struct Cfg {
	static constexpr float unitSize{ 32 };
	static constexpr int32_t unitLimit{ 100000 };
	static constexpr xx::FromTo<double> hpRange{ 25, 900 };

	static constexpr int32_t gridCellSize{ 32 };	// need >= max unit size
	static constexpr int32_t gridNumRows{ 22 };
	static constexpr int32_t gridNumCols{ 40 };

	static constexpr XY mapSize{ float(gridNumCols * gridCellSize), float(gridNumRows * gridCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
};
inline Cfg gCfg;
