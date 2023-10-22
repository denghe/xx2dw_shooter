#pragma once
#include <engine_base0.h>

struct Node {
	xx::List<xx::Shared<Node>, int32_t> children;
	xx::Weak<Node> parent;	// fill by MakeChildren

	SimpleAffineTransform trans;
	XY position{}, scale{ 1, 1 }, anchor{ 0.5, 0.5 }, size{};
	float alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool visible{ true };

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

	XX_FORCE_INLINE void Init() {
		FillTrans();
	}

	virtual void TransUpdate() {};
	virtual void Draw() {};									// draw current node only ( do not contain children )
	virtual ~Node() {};
};

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


/**********************************************************************************************/
/**********************************************************************************************/
/*
example:

void GameLooper::Draw() {
	FillZNodes(tmpZNodes, node);
	OrderByZDrawAndClear(tmpZNodes);

*/


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
	for (auto i = zns.len - 1; i < zns.len; ++i) {
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
