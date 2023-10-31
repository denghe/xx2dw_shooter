#pragma once
#include <engine_base0.h>

struct MouseEventHandlerNode : Node, SpaceGridABItem<MouseEventHandlerNode, XY> {
	virtual void OnMouseDown() = 0;
	virtual void OnMouseMove() = 0;
	virtual void OnMouseUp() = 0;

	XX_FORCE_INLINE bool MousePosInArea() const {
		auto p = trans.MakeInvert()(gEngine->mouse.pos);
		return PosInArea(p);
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
