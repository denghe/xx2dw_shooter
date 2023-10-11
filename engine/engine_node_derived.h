#pragma once
#include <engine_base0.h>

struct MouseEventHandlerNode : Node, SpaceGridABItem<MouseEventHandlerNode, XY> {
	virtual void OnMouseDown() = 0;
	virtual void OnMouseMove() = 0;
	virtual void OnMouseUp() = 0;

	bool MousePosInArea() {
		auto p = trans.MakeInvert()(gEngine->mouse.pos);
		return Calc::Intersects::BoxPoint({}, size, p);
	}

	virtual void TransUpdate() override {
		auto p = trans(size / 2);
		auto& hs = gEngine->mouseEventHandlers;
		SGABAddOrUpdate(hs, hs.max_2 + p, size * XY{trans.a, trans.d});
	}

	~MouseEventHandlerNode() {
		SGABRemove();
	}
};
