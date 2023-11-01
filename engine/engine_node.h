﻿#pragma once
#include <engine_base0.h>

struct Node {
	xx::List<xx::Shared<Node>, int32_t> children;
	xx::Weak<Node> parent;										// fill by MakeChildren
	xx::Weak<Node> scissor;										// fill by scroll view MakeContent

	union {
		SimpleAffineTransform trans{};
	struct {
		XY worldScale, worldMinXY;
	};
	};
	XY position{}, scale{ 1, 1 }, anchor{ 0.5, 0.5 }, size{};
	XY worldMaxXY{}, worldSize{};								// boundingBox. world coordinate. fill by FillTrans()
	float alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool inParentArea{ true };									// panel true ? combo box pop false ?


	// for init
	XX_FORCE_INLINE void FillTrans() {
		if (parent) {
			trans = SimpleAffineTransform::MakePosScaleAnchorSize(position, scale, anchor * size).MakeConcat(parent->trans);
		} else {
			trans.PosScaleAnchorSize(position, scale, anchor * size);
		}

		worldMaxXY = trans(size);
		worldSize = worldMaxXY - trans.Offset();

		TransUpdate();
	}

	// for draw FillZNodes
	XX_FORCE_INLINE bool IsVisible() const {
		if (scissor && !Calc::Intersects::BoxBox(worldMinXY, worldMaxXY, scissor->worldMinXY, scissor->worldMaxXY)) return false;
		if (!inParentArea) return true;
		if (parent) return Calc::Intersects::BoxBox(worldMinXY, worldMaxXY, parent->worldMinXY, parent->worldMaxXY);
		return Calc::Intersects::BoxBox(worldMinXY, worldMaxXY, gEngine->worldMinXY, gEngine->worldMaxXY);
	}

	// aabb point check with scissor
	XX_FORCE_INLINE bool PosInArea(XY const& pos) const {
		if (scissor && !Calc::Intersects::BoxPoint(scissor->worldMinXY, scissor->worldMaxXY, pos)) return false;
		return Calc::Intersects::BoxPoint(worldMinXY, worldMaxXY, pos);
	}

	// for draw bg
	XX_FORCE_INLINE XY CalcBorderSize(XY const& padding = {}) const {
		return size * scale + padding;
	}

	// for update
	void FillTransRecursive() {
		FillTrans();
		for (auto& c : children) {
			c->FillTransRecursive();
		}
	};

	void SetAlphaRecursive(float alpha_) {
		alpha = alpha_;
		for (auto& c : children) {
			c->SetAlphaRecursive(alpha_);
		}
	}

	void SetScissorRecursive(xx::Weak<Node> scissor_) {
		scissor = scissor_;
		for (auto& c : children) {
			c->SetScissorRecursive(scissor_);
		}
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Node, T>>>
	XX_FORCE_INLINE xx::Shared<T>& MakeChildren() {
		auto& r = children.Emplace().Emplace<T>();
		r->parent = xx::WeakFromThis(this);
		r->scissor = scissor;
		return r;
	}

	XX_FORCE_INLINE void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_ = { 0.5f, 0.5f }, XY const& scale_ = { 1,1 }) {
		z = z_;
		position = position_;
		scale = scale_;
		anchor = anchor_;
		size = size_;
		FillTrans();
	}

	XX_FORCE_INLINE void Init() {
		Init(0, {}, gEngine->windowSize, {});
	}

	virtual void TransUpdate() {};
	virtual void Draw() {};									// draw current node only ( do not contain children )
	virtual ~Node() {};
};

/**********************************************************************************************/
/**********************************************************************************************/

struct ZNode {
	decltype(Node::z) z;
	Node* n;
	XX_FORCE_INLINE Node* operator->() { return n; }
	inline XX_FORCE_INLINE static bool LessThanComparer(ZNode const& a, ZNode const& b) {
		return a.z < b.z;
	}
	inline XX_FORCE_INLINE static bool GreaterThanComparer(ZNode const& a, ZNode const& b) {
		return a.z > b.z;
	}
};

template<bool skipScissorContent = true>
inline void FillZNodes(xx::List<ZNode>& zns, Node* n) {
	assert(n);
	if constexpr (skipScissorContent) {
		if (n->scissor && n->scissor == n->parent) return;
	}
	if (!n->IsVisible()) return;
	zns.Emplace(n->z, n);
	for (auto& c : n->children) {
		FillZNodes(zns, c);
	}
}

inline void OrderByZDrawAndClear(xx::List<ZNode>& zns) {
	std::sort(zns.buf, zns.buf + zns.len, ZNode::LessThanComparer);	// draw small z first
	for (auto& zn : zns) {
		zn->Draw();
	}
	zns.Clear();
}

namespace xx {
	template<typename T>
	struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<Node, T>>> {
		static inline void Append(std::string& s, Node const& in) {
			::xx::Append(s, "{ trans = ", in.trans
				, ", position = ", in.position
				, ", scale = ", in.scale
				, ", anchor = ", in.anchor
				, ", size = ", in.size
				, " }"
			);
		}
	};
}
