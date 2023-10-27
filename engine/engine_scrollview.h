#pragma once
#include <engine_base2.h>

// todo: ScrollView move Directions
struct ScrollView : MouseEventHandlerNode, Scissor {
	std::function<void()> onDrawContent;	// todo: args -- basePos size scale ?

	void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_ = {0.5f, 0.5f}, XY const& scale_ = { 1,1 }) {
		z = z_;
		position = position_;
		scale = scale_;
		anchor = anchor_;
		size = size_;
		MouseEventHandlerNode::FillTrans();
	}

	template<typename F>
	void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_, XY const& scale_, F&& callback) {
		Init(z_, position_, scale_, anchor_, size_);
		onDrawContent = std::forward<F>(callback);
	}

	virtual void Draw() override {
		auto& eb = EngineBase1::Instance();
		XY pmin = trans, pmax{ trans(size) };
		auto wh = pmax - pmin;
		auto wh_2 = wh / 2.f;
		DrawTo(pmin, wh, onDrawContent);
		LineStrip().FillRectPoints(pmin, wh).Draw();
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
