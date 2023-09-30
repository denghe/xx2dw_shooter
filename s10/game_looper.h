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

	virtual void Visit(Node* parent) {
		if (!parent) return;
		at = parent->at.MakeConcat(at);
	};

	virtual void Draw() {};							// draw current node only ( do not contain children )
	virtual ~Node() {};

	// todo: AABB for cut? dirty?
	bool visible{ true };
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
	int i = zns.len;
	printf("i = %d\n", i);
	if (!n->visible) return;
	// todo: cut by AABB with camera? dirty?
	n->Visit({});
	zns.Emplace(n->z, n);
	while (i < zns.len) {
		printf("i = %d\n", i);
		n = zns[i].n;
		for (auto& c : n->children) {
			assert(c);
			if (!c->visible) continue;
			// todo: cut by AABB with camera?
			c->Visit(n);
			zns.Emplace(c->z, c.pointer);
		}
		++i;
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
	XY offset{};			// offset by parent
	std::string txt;	// todo: change to TinyFrame array?

	void Init(int z_, XY const& offset_, std::string_view const& txt_) {
		z = z_;
		offset = offset_;
		txt = txt_;
	}

	virtual void Visit(Node* parent) override {
		printf("Label Visit()\n");
		Node::Visit(parent);
	}

	virtual void Draw() override {
		printf("Label Draw()\n");
	}
};

struct Button : Node {
	XY offset{}, size{};
	std::function<void()> onClicked;

	void Init(int z_, XY const& offset_, XY const& size_, std::string_view const& txt_) {
		assert(size_.x > 6 && size_.y > 8);
		z = z_;
		offset = offset_;
		size = size_;
		children.Emplace().Emplace<Label>()->Init(z + 1, offset + XY{3, 3}, txt_);
	}

	virtual void Visit(Node* parent) override {
		printf("Button Visit()\n");
		Node::Visit(parent);
	}

	virtual void Draw() override {
		printf("Button Draw()\n");
	}
};
