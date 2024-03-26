#pragma once
#include "game_looper.h"

struct Obj {
	mutable Quad quad;
	XYi pos;

	virtual void Draw() const = 0;
	virtual ~Obj() {}
};
