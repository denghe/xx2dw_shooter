﻿#include "pch.h"
#include "looper.h"

xx::Task<> Looper::MainTask() {
	shapeRect.Emplace()->Init();

	ready = true;

	while (true) {
		for (float a = gPI / 4.f; a > -gPI / 4.f; a -= 0.01f * (60.f / gDesign.fps)) {
			shapeCircle.Emplace().Emplace()->Init(a);
			co_yield 0;
		}
		for (float a = -gPI / 4.f; a < gPI / 4.f; a += 0.01f * (60.f / gDesign.fps)) {
			shapeCircle.Emplace().Emplace()->Init(a);
			co_yield 0;
		}
	}
}

void Looper::Update() {
	if (!ready) return;
	shapeCircle.Foreach([](auto& o)->bool {
		return o->Update();
	});
	shapeRect->Update();
}

void Looper::Draw() {
	if (!ready) return;
	shapeCircle.Foreach([](auto& o)->void {
		o->Draw();
	});
	shapeRect->Draw();

	auto str = xx::ToString("total circle count = ", shapeCircle.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });
}



void ShapeCircle::Init(float radians_) {
	mainTask = MainTask();
	pos = { -400, 0 };
	radians = radians_;
	radius = 150;
	mainTask();
}


xx::Task<> ShapeCircle::MainTask() {
	XY inc{ std::cos(radians) * cSpeed, std::sin(radians) * cSpeed };
	for (size_t i = 0; i < gDesign.fps * 4; i++) {

		auto newPos = pos + inc;

		// find rect nearest point
		auto np = gLooper.shapeRect->GetNearPoint(newPos);

		// calc
		auto d = np - newPos;
		auto mag = std::sqrt(d.x * d.x + d.y * d.y);
		auto overlap = radius - mag;

		// intersect
		if (!std::isnan(overlap) && overlap > 0) {
			auto mag_1 = 1 / mag;
			auto p = d * mag_1 * overlap;

			// bounce
			if (np.x == newPos.x) {
				inc.y *= -1;
			} else if (np.y == newPos.y) {
				inc.x *= -1;
			} else {
				inc = (inc / cSpeed - d * mag_1).MakeNormalize() * cSpeed;	// todo: fix
			}

			newPos -= p;
		}

		// assign
		pos = newPos;

		co_yield 0;
	}
}

bool ShapeCircle::Update() {
	return mainTask.Resume();
}

void ShapeCircle::Draw() {
	LineStrip().FillCirclePoints({}, radius, {}, 80)
		.SetPosition(pos)
		.Draw();
}



void ShapeRect::Init() {
	pos = { 200, 0 };
	radius = 150;
	xy.from = pos - radius;
	xy.to = pos + radius;
}

XY ShapeRect::GetNearPoint(XY const& tar) {
	return { std::max(xy.from.x, std::min(tar.x, xy.to.x)),
	std::max(xy.from.y, std::min(tar.y, xy.to.y)) };
}

bool ShapeRect::Update() {
	return false;
}

void ShapeRect::Draw() {
	LineStrip().FillBoxPoints({}, XY::Make(radius*2))
		.SetPosition(pos)
		.Draw();
}
