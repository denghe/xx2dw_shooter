#pragma once
#include <engine_base2.h>

// todo: ScrollView move Directions

struct ScrollView : MouseEventHandlerNode, Scissor {

	xx::List<xx::Shared<Node>, int32_t> content;
	xx::List<ZNode> tmpZNodes;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Node, T>>>
	XX_FORCE_INLINE xx::Shared<T>& MakeContent() {
		auto& r = content.Emplace().Emplace<T>();
		r->parent = xx::WeakFromThis(this);
		return r;
	}

	virtual void Draw() override {
		DirectDrawTo(worldMinXY, worldSize, [&] {
			for (auto& n : content) {
				FillZNodes(tmpZNodes, n);
			}
			OrderByZDrawAndClear(tmpZNodes);
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
