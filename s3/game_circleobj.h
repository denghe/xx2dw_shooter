#pragma once
#include "game_obj.h"

struct CircleObj : Obj, SpaceGridCItem<CircleObj> {
	int radius{};
	~CircleObj() {
		SGCTryRemove();
	}
};
