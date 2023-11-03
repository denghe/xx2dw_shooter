#pragma once
#include <engine_base2.h>

// content align: left-top
struct ScrollView : MouseEventHandlerNode, Scissor {

	XY basePos{}, dragLimit{};

	void Init(int z_, XY const& position_, XY const& scale_, XY const& anchor_, XY const& size_, XY const& contentSize_) {
		Node::Init(z_, position_, scale_, anchor_, size_);
		children.Clear();
		auto&& c = MakeChildren<Node>();	// children[0] is content node
		c->scissor = xx::WeakFromThis(this);
		CalcDragLimit(contentSize_);
		c->Init(z_, basePos + dragLimit, {1,1}, {}, contentSize_);
	}

	template<typename T>
	XX_FORCE_INLINE xx::Shared<T>& MakeContent() {
		auto& c = children[0];
		assert(c->scissor.pointer() == this);
		auto& r = c->MakeChildren<T>();
		r->scissor = xx::WeakFromThis(this);
		return r;
	}

	template<bool resetPosition = true>
	XX_FORCE_INLINE void InitContentSize(XY const& contentSize_) {
		auto& c = children[0];
		assert(c->scissor.pointer() == this);
		if (c->size != contentSize_) {
			c->size = contentSize_;
			CalcDragLimit(contentSize_);
			if constexpr (resetPosition) {
				c->position = { basePos.x, basePos.y + dragLimit.y };
				c->FillTransRecursive();
			} else {
				UpdateContentPosition(c->position);
			}
		}
	}

	XX_FORCE_INLINE void CalcDragLimit(XY const& contentSize_) {
		if (size.y > contentSize_.y) {
			basePos = { 0, size.y - contentSize_.y };
		} else {
			basePos = {};
		}
		dragLimit.x = contentSize_.x > size.x ? -(contentSize_.x - size.x) : 0;
		dragLimit.y = contentSize_.y > size.y ? -(contentSize_.y - size.y) : 0;
	}

	XX_FORCE_INLINE bool UpdateContentPosition(XY pos) {
		auto& c = children[0];
		assert(c->scissor.pointer() == this);
		if (pos.x > 0) pos.x = 0;
		else if (pos.x < dragLimit.x) pos.x = dragLimit.x;
		if (pos.y > basePos.y) pos.y = basePos.y;
		else if (pos.y < basePos.y + dragLimit.y) pos.y = basePos.y + dragLimit.y;
		if (pos != c->position) {
			c->position = pos;
			c->FillTransRecursive();
			//xx::CoutN(pos);
			return true;
		}
		return false;
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
	}

	virtual void OnMouseMove() override {
		auto& eb = EngineBase1::Instance();
		if (eb.mouseEventHandler.pointer() == this) {
			auto d = eb.mouse.pos - mouseLastPos;
			mouseLastPos = eb.mouse.pos;
			UpdateContentPosition(children[0]->position + d);
		}
	}

	virtual void OnMouseUp() override {
		auto& eb = EngineBase1::Instance();
		assert(eb.mouseEventHandler.pointer() == this);
		eb.mouseEventHandler.Reset();
	}
};
