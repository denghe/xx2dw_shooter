#pragma once
#include "engine_node.h"

struct Scale9Sprite : Node {
	xx::Shared<Frame> frame;
	UVRect center;
	float texScale{ 1 };

	void Init(int z_, XY const& position_, float texScale_, XY const& size_, xx::Shared<Frame> frame_, UVRect const& center_) {
		assert(size_.x > 6 && size_.y > 8);
		z = z_;
		position = position_;
		size = size_;
		texScale = texScale_;
		frame = std::move(frame_);
		center = center_;
	}

	virtual void Draw() override {
		auto& s = gEngineBaseEx->shaderQuadInstance;
		if (gEngineBaseEx->shader != &s) {
			s.Begin();
		}
		auto& r = frame->textureRect;
		auto qs = s.Draw(frame->tex->GetValue(), 9);

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
		float px2 = center.x * texScale;
		float px3 = (center.x + center.w * sx) * texScale;

		float py1 = 0;
		float py2 = -float(center.y) * texScale;
		float py3 = -float(center.y + center.h * sy) * texScale;

		XY sc = scale * texScale;

		auto halfSize = size * texScale / 2;
		XY basePos{ -halfSize.x, halfSize.y };
		printf("%f %f \n", basePos.x, basePos.y);

		RGBA8 c = { 255, 255, 255, (uint8_t)(255 * alpha) };

		QuadInstanceData* q;
		q = &qs[0];
		q->pos = at.Apply(basePos + XY{ px1, py1 });
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty1, tw1, th1 };

		q = &qs[1];
		q->pos = at.Apply(basePos + XY{ px2, py1 });
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, 1 };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty1, tw2, th1 };

		q = &qs[2];
		q->pos = at.Apply(basePos + XY{ px3, py1 });
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty1, tw3, th1 };

		q = &qs[3];
		q->pos = at.Apply(basePos + XY{ px1, py2 });
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ 1, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty2, tw1, th2 };

		q = &qs[4];
		q->pos = at.Apply(basePos + XY{ px2, py2 });
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty2, tw2, th2 };

		q = &qs[5];
		q->pos = at.Apply(basePos + XY{ px3, py2 });
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ 1, sy };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty2, tw3, th2 };

		q = &qs[6];
		q->pos = at.Apply(basePos + XY{ px1, py3 });
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx1, ty3, tw1, th3 };

		q = &qs[7];
		q->pos = at.Apply(basePos + XY{ px2, py3 });
		q->anchor = { 0, 1 };
		q->scale = sc * XY{ sx, 1 };
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx2, ty3, tw2, th3 };

		q = &qs[8];
		q->pos = at.Apply(basePos + XY{ px3, py3 });
		q->anchor = { 0, 1 };
		q->scale = sc;
		q->radians = {};
		q->colormulti = { 1 };
		q->color = c;
		q->texRect = { tx3, ty3, tw3, th3 };
	}
};
