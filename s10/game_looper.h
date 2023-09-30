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
		//at.PosScaleRadiansAnchorSize(position, scale, radians, anchor * size);
		at.PosScale(position, scale);
		if (parent) {
			at = parent->at.MakeConcat(at);
		}
	};

	void SetAlpha(float alpha_) {
		alpha = alpha_;
		for (auto& c : children) {
			c->SetAlpha(alpha_);
		}
	}

	virtual void Draw() {};									// draw current node only ( do not contain children )
	virtual ~Node() {};

	XY position{}, scale{ 1,1 }, anchor{ 0.5, 0.5 }, size{};	// todo: calc AABB for cut?
	float radians{}, alpha{ 1 };
	int z{};												// global z for sort
	bool visible{ true };
	bool dirty{ true };										// for changed position, scale, anchor, size, radians
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

inline void PredrawAndFillTo(xx::List<ZNode>& zns, Node* n) {
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

	void Init(int z_, XY const& position_, std::string_view const& txt_) {
		z = z_;
		position = position_;
		txt = xx::StringU8ToU32(txt_);
		anchor = {};	// todo: do not use ctc.Draw
		size.x = gLooper.ctc72.Measure(txt);
		size.y = gLooper.ctc72.canvasHeight;
	}

	virtual void Draw() override {
		gLooper.ctc72.Draw(at.Apply(position) - XY{ size.x / 2, 0 }, txt);
	}
};

struct Scale9Sprite : Node {
	xx::Shared<Frame> frame;
	UVRect center;

	void Init(int z_, XY const& position_, XY const& size_, xx::Shared<Frame> frame_, UVRect const& center_) {
		assert(size_.x > 6 && size_.y > 8);
		z = z_;
		position = position_;
		size = size_;
		frame = std::move(frame_);
		center = center_;
	}

	virtual void Draw() override {
		auto& s = gLooper.shaderQuadInstance;
		if (gLooper.shader != &s) {
			s.Begin();
		}
		auto& r = frame->textureRect;
		auto qs = s.Draw(frame->tex->GetValue(), 9);

		XY sc{ 8, 8 };	// todo

		uint16_t tx1 = 0;
		uint16_t tx2 = center.x;
		uint16_t tx3 = center.x + center.w;

		uint16_t ty1 = 0;
		uint16_t ty2 = center.y;
		uint16_t ty3 = center.y + center.h;

		uint16_t tw1 = center.x;
		uint16_t tw2 = center.w;
		uint16_t tw3 = r.w - (center.x + center.w);

		uint16_t th1 = center.y;
		uint16_t th2 = center.h;
		uint16_t th3 = r.h - (center.y + center.h);

		float sx = float(size.x - tw1 - tw3) / tw2;
		float sy = float(size.y - th1 - th3) / th2;

		float px1 = 0;
		float px2 = center.x;
		float px3 = center.x + center.w * sx;

		float py1 = 0;
		float py2 = -float(center.y);
		float py3 = -float(center.y + center.h * sy);

		RGBA8 c = { 255, 255, 255, (uint8_t)(255 * alpha) };

		QuadInstanceData* q;
		q = &qs[0];
		q->pos = at.Apply(XY{ px1, py1 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty1, tw1, th1 };

		q = &qs[1];
		q->pos = at.Apply(XY{ px2, py1 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, 1 };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty1, tw2, th1 };

		q = &qs[2];
		q->pos = at.Apply(XY{ px3, py1 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty1, tw3, th1 };

		q = &qs[3];
		q->pos = at.Apply(XY{ px1, py2 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ 1, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty2, tw1, th2 };

		q = &qs[4];
		q->pos = at.Apply(XY{ px2, py2 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty2, tw2, th2 };

		q = &qs[5];
		q->pos = at.Apply(XY{ px3, py2 } * sc);
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ 1, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty2, tw3, th2 };

		q = &qs[6];
		q->pos = at.Apply(XY{ px1, py3 } *sc);
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty3, tw1, th3 };

		q = &qs[7];
		q->pos = at.Apply(XY{ px2, py3 } *sc);
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, 1 };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty3, tw2, th3 };

		q = &qs[8];
		q->pos = at.Apply(XY{ px3, py3 } *sc);
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty3, tw3, th3 };
	}
};

struct Button : Node {
	std::function<void()> onClicked;

	void Init(int z_, XY const& position_, XY const& size_, xx::Shared<Frame> frame_, UVRect const& center_, std::string_view const& txt_) {
		z = z_;
		position = position_;
		size = size_;
		children.Emplace().Emplace<Scale9Sprite>()->Init(z + 1, position, size_, std::move(frame_), center_);
		//children.Emplace().Emplace<Label>()->Init(z + 2, position + XY{3, 3}, txt_);
	}
};
