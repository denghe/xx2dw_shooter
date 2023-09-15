#pragma once
#include "game_looper.h"

struct Obj {
	mutable Quad quad;

	virtual void Draw() const = 0;
	virtual ~Obj() {}
};
