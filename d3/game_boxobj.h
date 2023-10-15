#pragma once
#include "game_obj.h"

struct BoxObj : Obj, SpaceGridABItem<BoxObj> {
	~BoxObj() {
		SGABRemove();
	}
};
