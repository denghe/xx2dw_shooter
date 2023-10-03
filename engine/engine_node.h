#pragma once
#include "engine_baseex.h"

struct Node {
	//xx::Weak<Node> parent;
	xx::List<xx::Shared<Node>, int32_t> children;

	XY position{}, scale{ 1,1 }, anchor{ 0.5, 0.5 }, size{};	// todo: AABB?
	float radians{}, alpha{ 1 };
	int z{};													// global z for event priority or batch combine
	bool visible{ true };
	bool dirty{ true };											// for changed position, scale, anchor, size, radians
	AffineTransform at{ AffineTransform::MakeIdentity() };

	virtual void Update(Node* parent) {
		//at.PosScaleRadiansAnchorSize(position, scale, radians, anchor * size);
		at.PosScale(position, scale);
		if (parent) {
			at = parent->at.MakeConcat(at);
		}
	};

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

	virtual void Draw() {};									// draw current node only ( do not contain children )
	virtual ~Node() {};
};

/**********************************************************************************************/
/**********************************************************************************************/

struct ZNode {
	decltype(Node::z) z;
	Node* n;
	XX_FORCE_INLINE Node* operator->() { return n; }
	inline XX_FORCE_INLINE static bool Comparer(ZNode const& a, ZNode const& b) {
		return a.z < b.z;
	}
};

inline void UpdateAndFillTo(xx::List<ZNode>& zns, Node* n) {
	assert(n);
	if (!n->visible) return;	// todo: cut by AABB with camera?
	if (n->dirty) {
		n->Update({});
	}
	zns.Emplace(n->z, n);
	for (int i = zns.len - 1; i < zns.len; ++i) {
		n = zns[i].n;
		for (auto& c : n->children) {
			assert(c);
			if (!c->visible) continue;	// todo: cut by AABB with camera?
			if (n->dirty) {
				c->dirty = true;
				c->Update(n);
			}
			zns.Emplace(c->z, c.pointer);
		}
		n->dirty = false;
	}
}

inline void OrderByZDrawAndClear(xx::List<ZNode>& zns) {
	std::sort(zns.buf, zns.buf + zns.len, ZNode::Comparer);
	for (auto& zn : zns) {
		zn->Draw();
	}
	zns.Clear();
}
