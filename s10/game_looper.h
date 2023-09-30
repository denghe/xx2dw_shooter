#pragma once
#include "game_looper_base.h"

struct Node;
struct ZNode;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	xx::Shared<Node> root;
	xx::List<ZNode> tmpZNodes;
};
extern GameLooper gLooper;

/**********************************************************************************************/
/**********************************************************************************************/

struct Node {
	//xx::Weak<Node> parent;
	xx::List<xx::Shared<Node>, int32_t> children;

	virtual void Predraw(Node* parent) {
		at.Pos(offset);
		if (parent) {
			at = parent->at.MakeConcat(at);
		}
	};

	virtual void Draw() {};							// draw current node only ( do not contain children )
	virtual ~Node() {};

	XY offset{}, size{};							// todo: calc AABB for cut?
	bool visible{ true }, dirty{ true };			// change offset args need set dirty
	float a{ 1 };
	int z{};
	AffineTransform at{ AffineTransform::MakeIdentity() };
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

inline void VisitAndFillTo(xx::List<ZNode>& zns, Node* n) {
	assert(n);
	if (!n->visible) return;	// todo: cut by AABB with camera?
	if (n->dirty) {
		n->Predraw({});
	}
	zns.Emplace(n->z, n);
	for (int i = zns.len - 1; i < zns.len; ++i) {
		n = zns[i].n;
		for (auto& c : n->children) {
			assert(c);
			if (!c->visible) continue;	// todo: cut by AABB with camera?
			if (n->dirty) {
				c->dirty = true;
				c->Predraw(n);
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

/**********************************************************************************************/
/**********************************************************************************************/

struct Label : Node {
	std::u32string txt;

	void Init(int z_, XY const& offset_, std::string_view const& txt_) {
		z = z_;
		offset = offset_;
		txt = xx::StringU8ToU32(txt_);
	}

	virtual void Draw() override {
		auto pos = at.Apply(offset);
		auto width = gLooper.ctc72.Measure(txt);
		gLooper.ctc72.Draw(pos - XY{width/2,0}, txt);
	}
};

struct Button : Node {
	std::function<void()> onClicked;

	void Init(int z_, XY const& offset_, XY const& size_, std::string_view const& txt_) {
		assert(size_.x > 6 && size_.y > 8);
		z = z_;
		offset = offset_;
		size = size_;
		children.Emplace().Emplace<Label>()->Init(z + 1, offset + XY{3, 3}, txt_);
	}

	virtual void Draw() override {
		// todo: Draw pic bg
	}
};
