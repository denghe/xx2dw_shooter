#pragma once
#include <engine_base2.h>

// todo: ScrollView move Directions

struct ScrollView : MouseEventHandlerNode, Scissor {

	void Init(int z_, XY const& position_, XY const& scale_, XY const& anchor_, XY const& size_, XY const& contentSize_) {
		Node::Init(z_, position_, scale_, anchor_, size_);
		children.Clear();
		auto&& c = MakeChildren<Node>();	// children[0] is content node
		c->scissor = xx::WeakFromThis(this);
		c->Init(z_, {}, scale_, {}, contentSize_);
		// todo: if contentSize_ < size_ ??? calc drag area
	}

	template<typename T>
	XX_FORCE_INLINE xx::Shared<T>& MakeContent() {
		assert(children.len);
		auto& r = children[0]->MakeChildren<T>();
		r->scissor = xx::WeakFromThis(this);
		return r;
	}

	xx::List<ZNode> tmpZNodes;
	virtual void Draw() override {
		DirectDrawTo(worldMinXY, worldSize, [&] {
			FillZNodes<false>(tmpZNodes, children[0]);
			OrderByZDrawAndClear(tmpZNodes);
		});
	};

	XY mouseDownPos{}, mouseLastPos{};
	virtual void OnMouseDown() override {
		auto& eb = EngineBase1::Instance();
		assert(!eb.mouseEventHandler);
		eb.mouseEventHandler = xx::WeakFromThis(this);
		mouseLastPos = mouseDownPos = eb.mouse.pos;
		xx::CoutN("OnMouseDown() ", eb.mouse.pos);
	}

	virtual void OnMouseMove() override {
		auto& eb = EngineBase1::Instance();
		if (eb.mouseEventHandler.pointer() == this) {
			auto d = eb.mouse.pos - mouseLastPos;
			mouseLastPos = eb.mouse.pos;
			auto& c = children[0];
			c->position += d;
			c->FillTransRecursive();
			xx::CoutN("c->position ", c->position);
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
