#pragma once
#include <engine_node.h>

struct Scale9Sprite : Node {
	xx::Shared<Frame> frame;
	UVRect center;
	RGBA8 color;
	float texScale;
	float colorplus;

	void Init(int z_, XY const& position_, XY const& anchor_, float texScale_, XY const& size_, xx::Shared<Frame> frame_, UVRect const& center_, RGBA8 color_ = RGBA8_White, float colorplus_ = 1) {
		assert(size_.x > 6 && size_.y > 8);
		z = z_;
		position = position_;
		size = size_;
		anchor = anchor_;
		texScale = texScale_;
		frame = std::move(frame_);
		center = center_;
		color = color_;
		colorplus = colorplus_;

		FillTrans();
	}

	virtual void Draw() override {
		auto qs = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance).Draw(frame->tex->GetValue(), 9);

		auto& r = frame->textureRect;

		uint16_t tx1 = r.x + 0;
		uint16_t tx2 = r.x + center.x;
		uint16_t tx3 = r.x + center.x + center.w;

		uint16_t ty1 = r.y + 0;
		uint16_t ty2 = r.y + center.y;
		uint16_t ty3 = r.y + center.y + center.h;

		uint16_t tw1 = center.x;
		uint16_t tw2 = center.w;
		uint16_t tw3 = r.w - (center.x + center.w);

		uint16_t th1 = center.y;
		uint16_t th2 = center.h;
		uint16_t th3 = r.h - (center.y + center.h);

		// actual
		XY siz{ size.x * trans.a, size.y * trans.d };
		XY ts{ texScale * trans.a, texScale * trans.d };

		float sx = float(siz.x - tw1 * ts.x - tw3 * ts.x) / tw2;
		float sy = float(siz.y - th1 * ts.y - th3 * ts.y) / th2;
#ifndef NDEBUG
		if (sx < 0 || sy < 0) {
			xx::CoutN(" sx = ", sx, " sy = ", sy, " ts = ", ts);
			xx_assert(false);
		}
#endif

		float px1 = 0;
		float px2 = tw1 * ts.x;
		float px3 = siz.x - tw3 * ts.x;

		float py1 = siz.y;
		float py2 = siz.y -(th1 * ts.y);
		float py3 = siz.y -(siz.y - th3 * ts.y);

		XY basePos = trans;

		RGBA8 c = { color.r, color.g, color.b, (uint8_t)(color.a * alpha) };

		QuadInstanceData* q;
		q = &qs[0];
		q->pos = basePos + XY{ px1, py1 };
		q->anchor = { 0, 1 };
		q->scale = ts;
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx1, ty1, tw1, th1 };

		q = &qs[1];
		q->pos = basePos + XY{ px2, py1 };
		q->anchor = { 0, 1 };
		q->scale = { sx, ts.y };
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx2, ty1, tw2, th1 };

		q = &qs[2];
		q->pos = basePos + XY{ px3, py1 };
		q->anchor = { 0, 1 };
		q->scale = ts;
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx3, ty1, tw3, th1 };

		q = &qs[3];
		q->pos = basePos + XY{ px1, py2 };
		q->anchor = { 0, 1 };
		q->scale = { ts.x, sy };
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx1, ty2, tw1, th2 };

		q = &qs[4];
		q->pos = basePos + XY{ px2, py2 };
		q->anchor = { 0, 1 };
		q->scale = { sx, sy };
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx2, ty2, tw2, th2 };

		q = &qs[5];
		q->pos = basePos + XY{ px3, py2 };
		q->anchor = { 0, 1 };
		q->scale = { ts.x, sy };
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx3, ty2, tw3, th2 };

		q = &qs[6];
		q->pos = basePos + XY{ px1, py3 };
		q->anchor = { 0, 1 };
		q->scale = ts;
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx1, ty3, tw1, th3 };

		q = &qs[7];
		q->pos = basePos + XY{ px2, py3 };
		q->anchor = { 0, 1 };
		q->scale = { sx, ts.y };
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx2, ty3, tw2, th3 };

		q = &qs[8];
		q->pos = basePos + XY{ px3, py3 };
		q->anchor = { 0, 1 };
		q->scale = ts;
		q->radians = {};
		q->colorplus = colorplus;
		q->color = c;
		q->texRect = { tx3, ty3, tw3, th3 };
	}
};
