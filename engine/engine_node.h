#pragma once
#include "engine_base0.h"

struct Node {
	xx::List<xx::Shared<Node>, int32_t> children;
	xx::Weak<Node> parent;	// fill by MakeChildren

	/*AffineTransform*/XY trans{};
	XY position{}, anchor{ 0.5, 0.5 }, size{};
	float alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool visible{ true };

	// for init
	XX_FORCE_INLINE void FillTrans() {
		if (parent) {
			trans = parent->trans + position - anchor * size;
		} else {
			trans = position - anchor * size;
		}
		TransUpdate();
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
	xx::Shared<T>& MakeChildren() {
		auto& r = children.Emplace().Emplace<T>();
		r->parent = xx::WeakFromThis(this);
		return r;
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
	if (!n->visible) return;
	zns.Emplace(n->z, n);
	for (int i = zns.len - 1; i < zns.len; ++i) {
		n = zns[i].n;
		for (auto& c : n->children) {
			assert(c);
			if (!c->visible) continue;
			zns.Emplace(c->z, c.pointer);
		}
	}
}

inline void OrderByZDrawAndClear(xx::List<ZNode>& zns) {
	std::sort(zns.buf, zns.buf + zns.len, ZNode::LessThanComparer);	// draw small z first
	for (auto& zn : zns) {
		// todo: cut by AABB with camera?
		zn->Draw();
	}
	zns.Clear();
}
