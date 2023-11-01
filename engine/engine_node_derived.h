#pragma once
#include <engine_base0.h>

struct MouseEventHandlerNode : Node, SpaceGridABItem<MouseEventHandlerNode, XY> {
	virtual void OnMouseDown() = 0;
	virtual void OnMouseMove() = 0;
	virtual void OnMouseUp() = 0;

	XX_FORCE_INLINE bool MousePosInArea() const {		// virtual ?
		return PosInArea(gEngine->mouse.pos);
	}

	virtual void TransUpdate() override {
		auto& hs = gEngine->mouseEventHandlers;
		SGABAddOrUpdate(hs, hs.max_2 + worldMinXY + worldSize / 2, worldSize);
	}

	~MouseEventHandlerNode() {
		SGABRemove();
	}
};
