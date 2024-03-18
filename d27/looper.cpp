#include "pch.h"
#include "looper.h"

xx::Task<> Looper::MainTask() {
	shapeRect.Emplace()->Init();

	ready = true;

	tasks.Add([&]()->xx::Task<> {
		while (true) {
			for (float a = gPI / 4.f; a > -gPI / 4.f; a -= 0.01f * (60.f / gDesign.fps)) {
				shapeCircle.Emplace().Emplace()->Init({ -400, 0 }, a);
				co_yield 0;
			}
			for (float a = -gPI / 4.f; a < gPI / 4.f; a += 0.01f * (60.f / gDesign.fps)) {
				shapeCircle.Emplace().Emplace()->Init({ -400, 0 }, a);
				co_yield 0;
			}
		}
	});
	tasks.Add([&]()->xx::Task<> {
		while (true) {
			for (float a = gPI / 4.f; a > -gPI / 4.f; a -= 0.01f * (60.f / gDesign.fps)) {
				shapeCircle.Emplace().Emplace()->Init({ 400, 0 }, a + gPI);
				co_yield 0;
			}
			for (float a = -gPI / 4.f; a < gPI / 4.f; a += 0.01f * (60.f / gDesign.fps)) {
				shapeCircle.Emplace().Emplace()->Init({ 400, 0 }, a + gPI);
				co_yield 0;
			}
		}
	});
	co_yield 0;
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

	auto str = xx::ToString("The box can be dragged with the mouse. total circle count = ", shapeCircle.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });
}



void ShapeCircle::Init(XY const& pos_, float radians_) {
	mainTask = MainTask();
	pos = pos_;
	radians = radians_;
	radius = 150;
	mainTask();
}


xx::Task<> ShapeCircle::MainTask() {
	XY inc{ std::cos(radians) * cSpeed, std::sin(radians) * cSpeed };
	for (size_t i = 0; i < gDesign.fps * 4; i++) {

		auto newPos = pos + inc;

		// find rect nearest point
		auto np = gLooper.shapeRect->GetNearestPoint(newPos);

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
				auto a1 = std::atan2(-inc.y, -inc.x);
				auto a2 = std::atan2(-p.y, -p.x);
				auto gap = RotateControl::Gap(a1, a2);
				a2 += gap;	// todo?
				inc = XY{ std::cos(a2) * cSpeed, std::sin(a2) * cSpeed };
			}
			// todo: bugfix when box pos ~= circle pos

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
	pos = { 0, 0 };
	radius = 80;
	xy.from = pos - radius;
	xy.to = pos + radius;
}

XY ShapeRect::GetNearestPoint(XY const& tar) {
	return { std::max(xy.from.x, std::min(tar.x, xy.to.x)),
	std::max(xy.from.y, std::min(tar.y, xy.to.y)) };
}

bool ShapeRect::Update() {
	if (!gLooper.mouseEventHandler) {
		auto& m = gLooper.mouse;
		auto mbs = m.btnStates[0];
		if (lastMBState != mbs) {
			lastMBState = mbs;
			if (mbs) {	// mouse down
				if (Calc::Intersects::BoxPoint(xy.from, xy.to, m.pos)) {
					dragging = true;
					mouseOffset = m.pos - pos;
				}
			} else {	// mouse up
				dragging = false;
			}
		}
		if (dragging) {
			pos = m.pos - mouseOffset;
			xy.from = pos - radius;
			xy.to = pos + radius;
		}
	} else {
		dragging = false;
	}
	return false;
}

void ShapeRect::Draw() {
	LineStrip().FillBoxPoints({}, XY::Make(radius*2))
		.SetPosition(pos)
		.Draw();
}
