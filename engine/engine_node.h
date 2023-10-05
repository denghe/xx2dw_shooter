#pragma once
#include "engine_baseex.h"

struct Node {
	//xx::Weak<Node> parent;
	xx::List<xx::Shared<Node>, int32_t> children;

	XY position{}, scale{ 1,1 }, anchor{ 0.5, 0.5 }, size{};
	float alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool visible{ true };
	bool dirty{ true };											// for changed position, scale, anchor, size
	AffineTransform trans{ AffineTransform::MakeIdentity() };

	template<bool hasParent = true>
	void Update(Node* parent) {
		if (!visible || !dirty) return;
		if constexpr (hasParent) {
			trans = parent->trans.MakeConcat(trans.MakePosScaleAnchorSize(position, scale, anchor * size));
		} else {
			trans.PosScaleAnchorSize(position, scale, anchor * size);
		}
		DerivedUpdate();
		for (auto& c : children) {
			if (!c->visible) continue;
			c->dirty = true;
			c->Update(this);
		}
		dirty = false;
	};

	XX_FORCE_INLINE void Update() {
		Update<false>({});
	}

	void SetAlphaRecursive(float alpha_) {
		alpha = alpha_;
		for (auto& c : children) {
			c->SetAlphaRecursive(alpha_);
		}
	}

	void SetPosition(XY const& xy) { if (position != xy) { position = xy; dirty = true; } }
	void SetPositionX(float x) { if (position.x != x) { position.x = x; dirty = true; } }
	void SetPositionY(float y) { if (position.y != y) { position.y = y; dirty = true; } }
	void SetScale(XY const& xy) { if (scale != xy) { scale = xy; dirty = true; } }
	void SetScaleX(float x) { if (scale.x != x) { scale.x = x; dirty = true; } }
	void SetScaleY(float y) { if (scale.y != y) { scale.y = y; dirty = true; } }
	void SetAnchor(XY const& xy) { if (anchor != xy) { anchor = xy; dirty = true; } }
	void SetAnchoreX(float x) { if (anchor.x != x) { anchor.x = x; dirty = true; } }
	void SetAnchoreY(float y) { if (anchor.y != y) { anchor.y = y; dirty = true; } }
	void SetSize(XY const& xy) { if (size != xy) { size = xy; dirty = true; } }
	void SetSizeX(float x) { if (size.x != x) { size.x = x; dirty = true; } }
	void SetSizeY(float y) { if (size.y != y) { size.y = y; dirty = true; } }

	virtual void DerivedUpdate() {};
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
