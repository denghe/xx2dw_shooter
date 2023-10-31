#pragma once
#include <engine_base0.h>

struct Node {
	xx::List<xx::Shared<Node>, int32_t> children;
	xx::Weak<Node> parent;										// fill by MakeChildren

	SimpleAffineTransform trans;
	XY position{}, scale{ 1, 1 }, anchor{ 0.5, 0.5 }, size{};
	XY worldMinXY{}, worldMaxXY{}, worldSize;					// boundingBox. world coordinate. fill by FillTrans()
	float alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool visible{ true };										// false: custom handle
	bool cutByParent{ true };									// parent is scissor ?

	XX_FORCE_INLINE XY CalcBorderSize(XY const& padding = {}) const {
		return size * scale + padding;
	}

	// for init
	XX_FORCE_INLINE void FillTrans() {
		if (parent) {
			trans = SimpleAffineTransform::MakePosScaleAnchorSize(position, scale, anchor * size).MakeConcat(parent->trans);
		} else {
			trans.PosScaleAnchorSize(position, scale, anchor * size);
		}

		worldMinXY = trans;
		worldMaxXY = trans(size);
		worldSize = worldMaxXY - worldMinXY;

		TransUpdate();
	}

	// visible + cut check
	XX_FORCE_INLINE bool IsVisible() {
		if (!visible) return false;
		if (!cutByParent) return true;
		if (parent) return Calc::Intersects::BoxBox(worldMinXY, worldMaxXY, parent->worldMinXY, parent->worldMaxXY);
		return Calc::Intersects::BoxBox(worldMinXY, worldMaxXY, gEngine->worldMinXY, gEngine->worldMaxXY);
	}

	// for update
	void FillTransRecursive() {
		if (!visible) return;
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

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Node, T>>>
	XX_FORCE_INLINE xx::Shared<T>& MakeChildren() {
		auto& r = children.Emplace().Emplace<T>();
		r->parent = xx::WeakFromThis(this);
		return r;
	}

	void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_ = { 0.5f, 0.5f }, XY const& scale_ = { 1,1 }) {
		z = z_;
		position = position_;
		scale = scale_;
		anchor = anchor_;
		size = size_;
		FillTrans();
	}

	XX_FORCE_INLINE void Init() {
		FillTrans();
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

inline void FillZNodes(xx::List<ZNode>& zns, Node* n) {
	assert(n);
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
