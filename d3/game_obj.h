#pragma once
#include "game_looper.h"

struct Obj {
	mutable Quad quad;
	Vec2<> pos;

	virtual void Draw() const = 0;
	virtual ~Obj() {}
};
