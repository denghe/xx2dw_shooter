#pragma once
#include <engine_prims.h>

struct Cfg {
	static constexpr float unitSize{ 32 };
	static constexpr double unitHP{ 100 };
	static constexpr int32_t unitLimit{ 100000 };
	static constexpr xx::FromTo<double> hpRange{ 25, 200 };	// 900		// for test1
	static constexpr xx::FromTo<double> hpRange2{ 25, 100 };			// for test2
	static constexpr int32_t gridCellSize{ (int32_t)(unitSize * hpRange.to / unitHP) };	// need >= max unit size	// for test1

	static constexpr XY cameraOriginal{ unitSize * 20, unitSize * 11 };

	//static constexpr int32_t gridNumRows{ 22 };
	//static constexpr int32_t gridNumCols{ 40 };

	//static constexpr XY mapSize{ float(gridNumCols * gridCellSize), float(gridNumRows * gridCellSize) };
	//static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };




};
inline Cfg gCfg;


// todo: sqlite cfg db -> this file content
