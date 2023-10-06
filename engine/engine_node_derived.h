﻿#pragma once
#include "engine_base0.h"

struct MouseEventHandlerNode : Node, SpaceGridABItem<MouseEventHandlerNode, XY> {
	virtual void OnMouseDown() = 0;
	virtual void OnMouseMove() = 0;
	virtual void OnMouseUp() = 0;

	bool MousePosInArea() {
		auto p = gEngine->mouse.pos - trans;
		return Calc::Intersects::BoxPoint({}, size, p);
	}

	virtual void TransUpdate() override {
		auto pos = trans + size / 2;
		auto& hs = gEngine->mouseEventHandlers;
		SGABAddOrUpdate(hs, hs.max_2 + pos, size);
	}

	~MouseEventHandlerNode() {
		SGABRemove();
	}
};
