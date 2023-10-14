#pragma once
#include <engine_quad.h>

struct RepeatQuad {
	Quad q;
	XY from{}, to{};
	float texOffsetXPercentage{};

	template<bool forceOverrideTexRectId = false>
	XX_FORCE_INLINE RepeatQuad& SetFrame(xx::Shared<Frame> f) {
		q.SetFrame<forceOverrideTexRectId>(std::move(f));
		return *this;
	}

	XX_FORCE_INLINE RepeatQuad& SetScale(XY const& s) {
		q.scale = s;
		return *this;
	}

	XX_FORCE_INLINE RepeatQuad& SetFrom(XY const& p) {
		from = p;
		return *this;
	}

	XX_FORCE_INLINE RepeatQuad& SetTo(XY const& p) {
		to = p;
		return *this;
	}

	XX_FORCE_INLINE RepeatQuad& SetFromTo(XY const& p1, XY const& p2) {
		from = p1;
		to = p2;
		return *this;
	}

	XX_FORCE_INLINE RepeatQuad& SetTexOffsetXPercentage(float p) {
		texOffsetXPercentage = p;
		return *this;
	}

	void Draw() {
		assert(texOffsetXPercentage >= 0 && texOffsetXPercentage <= 1);
		auto v = to - from;								// vector
		auto d = std::sqrt(v.x * v.x + v.y * v.y);		// distance
		if (std::abs(d) < std::numeric_limits<float>::epsilon()) return;
		auto tr = q.frame->textureRect;
		auto stw = tr.w * q.scale.x;					// tex scaled width
		q.anchor = { 0, 0.5 };
		q.pos = from;
		q.radians = -std::atan2(v.y, v.x);
		XY inc{ v.x / d, v.y / d };						// normalize
		auto inc_tw = inc * stw;
		if (texOffsetXPercentage > 0) {
			auto tw = uint32_t(tr.w * texOffsetXPercentage);
			if (d <= stw * texOffsetXPercentage) {
				tw = uint32_t(d / stw * tr.w);
			}
			if (tw) {
				q.texRect.w = tw + 1;
				q.texRect.x = tr.x + tr.w - tw;
				q.Draw();
				d -= stw * texOffsetXPercentage;
				if (d < std::numeric_limits<float>::epsilon()) return;
				q.pos += inc_tw * texOffsetXPercentage;
			}
		}
		q.texRect.x = tr.x;
		if (d > stw) {
			int n = int(d / stw);						// num instances
			q.texRect.w = tr.w;
			for (int i = 0; i < n; ++i, q.pos += inc_tw) {
				q.Draw();
			}
			d -= stw * n;								// sync left distance
			assert(d >= 0);
		}
		q.texRect.w = uint16_t(tr.w * d / stw);			// cut texture
		if (q.texRect.w > 0) {
			q.Draw();
		}
	}
};
