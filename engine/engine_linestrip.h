#pragma once
#include "engine_base.h"

struct LineStrip {
	std::vector<XYRGBA8> pointsBuf;
	AffineTransform at;
	bool dirty = true;

	std::vector<XY> points;
	XY size{};
	XY pos{};
	XY anchor{ 0.5, 0.5 };
	XY scale{ 1, 1 };
	float radians{};
	RGBA8 color{ 255, 255, 255, 255 };

	std::vector<XY>& SetPoints() {
		dirty = true;
		return points;
	}
	LineStrip& SetPoints(std::initializer_list<XY> ps) {
		dirty = true;
		points = ps;
		return *this;
	}
	template<bool loop = false, typename A>
	LineStrip& SetPointsArray(A const& a) {
		dirty = true;
		points.clear();
		points.insert(points.begin(), a.begin(), a.end());
		if constexpr (loop) {
			points.push_back(*a.begin());
		}
		return *this;
	}

	LineStrip& FillCirclePoints(XY const& center, float const& radius, std::optional<float> const& angle = {}, int const& segments = 100, XY const& scale = { 1,1 }) {
		dirty = true;
		points.reserve(segments + 2);
		points.resize(segments + 1);
		auto coef = 2.0f * (float)M_PI / segments;
		auto a = angle.has_value() ? *angle : 0;
		for (int i = 0; i <= segments; ++i) {
			auto rads = i * coef + a;
			points[i].x = radius * cosf(rads) * scale.x + center.x;
			points[i].y = radius * sinf(rads) * scale.y + center.y;
		}
		if (angle.has_value()) {
			points.push_back(center);
		}
		return *this;
	}

	LineStrip& FillBoxPoints(XY const& center, XY const& wh) {
		auto hwh = wh / 2;
		points.resize(5);
		points[0] = { -hwh.x, -hwh.y };
		points[1] = { -hwh.x,hwh.y };
		points[2] = { hwh.x,hwh.y };
		points[3] = { hwh.x,-hwh.y };
		points[4] = { -hwh.x,-hwh.y };
		return *this;
	}

	// ... more?

	LineStrip& Clear() {
		dirty = true;
		points.clear();
		return *this;
	}

	LineStrip& SetSize(XY const& s) {
		dirty = true;
		size = s;
		return *this;
	}

	LineStrip& SetAnchor(XY const& a) {
		dirty = true;
		anchor = a;
		return *this;
	}

	LineStrip& SetRotate(float const& r) {
		dirty = true;
		radians = r;
		return *this;
	}

	LineStrip& SetScale(XY const& s) {
		dirty = true;
		scale = s;
		return *this;
	}
	LineStrip& SetScale(float const& s) {
		dirty = true;
		scale = { s, s };
		return *this;
	}

	LineStrip& SetPosition(XY const& p) {
		dirty = true;
		pos = p;
		return *this;
	}

	LineStrip& SetColor(RGBA8 const& c) {
		dirty = true;
		color = c;
		return *this;
	}
	LineStrip& SetColorAf(float const& a) {
		dirty = true;
		color.a = 255 * a;
		return *this;
	}
	LineStrip& SetColorA(uint8_t const& a) {
		dirty = true;
		color.a = a;
		return *this;
	}

	void Update() {
		if (dirty) {
			auto&& ps = points.size();
			pointsBuf.resize(ps);
			at = at.MakePosScaleRadiansAnchorSize(pos, scale, radians, size * anchor);
			for (size_t i = 0; i < ps; ++i) {
				(XY&)pointsBuf[i].x = at.Apply(points[i]);
				memcpy(&pointsBuf[i].r, &color, sizeof(color));
			}
			dirty = false;
		}
	}

	void Draw() {
		Update();
		if (auto&& ps = pointsBuf.size()) {
			gEngineBase->ShaderBegin(gEngineBase->shaderLineStrip).Draw(pointsBuf.data(), ps);
		}
	}
};
