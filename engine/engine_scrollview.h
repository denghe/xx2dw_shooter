#pragma once
#include <engine_base2.h>

// todo: ScrollView move Directions

struct ScrollView : MouseEventHandlerNode, Scissor {
	XY basePos{}, scaledSize{};	// todo: replace to boundingBox

	void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_ = {0.5f, 0.5f}, XY const& scale_ = { 1,1 }) {
		z = z_;
		position = position_;
		scale = scale_;
		anchor = anchor_;
		size = size_;
		SyncBasePosScaledSize();
	}

	XX_FORCE_INLINE void SyncBasePosScaledSize() {
		Node::FillTrans();
		basePos = trans;
		XY pmax{ trans(size) };
		scaledSize = pmax - basePos;
	}

	// todo: MakeChildren set scissor by parent?

	// scan child z & set visible ( call after all children add )
	void MakeChildrenEnd() {
		for (auto& n : children) {
			n->visible = n->z < z;
		}
	}

	virtual void Draw() override {
		DirectDrawTo(basePos, scaledSize, [&] {
			auto& eb = EngineBase1::Instance();
			for (auto& n : children) {
				if (n->visible) continue;
				FillZNodes<true>(eb.tmpZNodes2, n);
			}
			OrderByZDrawAndClear(eb.tmpZNodes2);
		});
	};

	// todo: mouse event dispatch to content

	virtual void OnMouseDown() override {
		auto& eb = EngineBase1::Instance();
		assert(!eb.mouseEventHandler);
		eb.mouseEventHandler = xx::WeakFromThis(this);
		// todo
		xx::CoutN("OnMouseDown() ", eb.mouse.pos);
	}

	virtual void OnMouseMove() override {
		auto& eb = EngineBase1::Instance();
		if (eb.mouseEventHandler.pointer() != this/* && !changePos*/) {
			/*changePos(gEngine->tasks, ChangePos());*/
			xx::CoutN("OnMouseMove() ", eb.mouse.pos);
		}
	}

	virtual void OnMouseUp() override {
		auto& eb = EngineBase1::Instance();
		assert(eb.mouseEventHandler.pointer() == this);
		eb.mouseEventHandler.Reset();
		// todo
		xx::CoutN("OnMouseUp() ", eb.mouse.pos);
	}

	xx::TaskGuard changePos;
	xx::Task<> ChangePos() {
		// todo
		co_return;
	}
};
