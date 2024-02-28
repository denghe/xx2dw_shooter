#include <pch.h>
#include <all.h>

#pragma region Eye

void Eye::Init(EyeBase& prev_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_) {
	// init args
	prev = &prev_;
	cRadiansRange = cRadiansRange_;
	cDistanceRange = cDistanceRange_;
	scale = cScale;
	radius = cRadius;
	radians = prev->radians;
	pos = prev->pos;
	Update();
}

// limit a by from ~ to
// no change: return false
inline XX_FORCE_INLINE bool Limit(float& a, float from, float to) {
	// -PI ~~~~~~~~~~~~~~~~~~~~~~~~~ a ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PI
    assert(a >= gNPI && a <= gPI);
	// -PI ~~~~~~~~ from ~~~~~~~~~~~~~~ a ~~~~~~~~~~~~~~~~ to ~~~~~~~~~~~~ PI
	if (a >= from && a <= to) return false;
	// from ~~~~~~~~~~~~~~~ -PI ~~~~~~~~~~~~~~~~~~ to ~~~~~~~~~~~~~~~ PI
	if (from < gNPI) {
		// ~~~~~~ from ~~~~~~~ -PI ~~~~~~ to ~~~~ a ~~~~ 0 ~~~~~~~~~~~~~ PI
		if (a < 0) {
			if (a - to < from + g2PI - a) {
				a = to;
			} else {
				a = from + g2PI;
			}
		// ~~~~~ d ~~~~~~ from ~~~~~~~ -PI ~~~~~~~~ to ~~~~~~~~ 0 ~~~~~~~ a ~~~~ PI
		} else {
			auto d = a - g2PI;
			if (d >= from && d <= to) return false;
			else {
				if (from - d < a - to) {
					a = from + g2PI;
				} else {
					a = to;
				}
			}
		}
	// -PI ~~~~~~~~~~ from ~~~~~~~~~~~~~ PI ~~~~~~~ to
	} else if (to > gPI) {
		// -PI ~~~~~~~~~~~~~~~ 0 ~~~~~ a ~~~~~ from ~~~~~~ PI ~~~~~~~ to
		if (a > 0) {
			if (from - a < a - (to - g2PI)) {
				a = from;
			} else {
				a = to - g2PI;
			}
		// -PI ~~~~~~~ a ~~~~~~~~ 0 ~~~~~~~ from ~~~~~~ PI ~~~~~~~ to ~~~~~ d ~~~~~
		} else {
			auto d = a + g2PI;
			if (d >= from && d <= to) return false;
			else {
				if (from - a < d - to) {
					a = from;
				} else {
					a = to - g2PI;
				}
			}
		}
	} else {
		// -PI ~~~~~ a ~~~~ from ~~~~~~~~~~~~~~~~~~ to ~~~~~~~~~~~ PI
		if (a < from) {
			a = from;
		// -PI ~~~~~~~~~ from ~~~~~~~~~~~~~~~~~~ to ~~~~~ a ~~~~~~ PI
		} else {
			a = to;
		}
	}
    return true;	// todo: bug fix. handle: to - from > PI?
}

void Eye::Update() {
	radians = std::atan2(pos.y - prev->pos.y, pos.x - prev->pos.x);
	Limit(radians, prev->radians + cRadiansRange.from, prev->radians + cRadiansRange.to);
	pos.x = prev->pos.x + std::cos(radians) * cDistanceRange.from;
	pos.y = prev->pos.y + std::sin(radians) * cDistanceRange.from;
}

void Eye::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
}

#pragma endregion

#pragma region BigEye

void BigEye::Init(XY const& pos_) {
	// init args
	pos = {};
	scale = cScale;
	radius = cRadius;
	radians = {};

	// init children
	static constexpr int numEyess{ 20 }, numEyes{ 30 };
	static constexpr float step{ g2PI / numEyess };
	eyess.Resize(numEyess);
	for (int i = 0; i < numEyess; i++) {
		auto& eyes = eyess[i];
		eyes.Resize(numEyes);
		auto r = step * i;
		eyes[0].Init(*this, {r, r}, { radius, radius });	// todo
		for (int j = 1; j < numEyes; j++) {
			eyes[j].Init(eyes[j - 1], {-0.2f, 0.2f}, { Eye::cRadius, Eye::cRadius });	// todo
		}
	}
}

void BigEye::Update(Camera const& camera) {
	auto& m = gLooper.mouse;
	if (m.btnStates[0] && !gLooper.mouseEventHandler) {
		// follow mouse
		auto p = camera.ToLogicPos(m.pos);
		auto v = p - pos;
		auto distance = std::sqrt(v.x * v.x + v.y * v.y);
		if (auto d = distance - cSpeed; d > 0) {
			auto inc = v / distance * cSpeed;
			pos = pos + inc;
		} else {
			pos = p;
		}
	}

	for (auto& eyes : eyess) {
		for (auto& eye : eyes) {
			eye.Update();
		}
	}
}

void BigEye::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = 0;	// fixed radians
	q.colorplus = 1;
	q.color = RGBA8_White;

	for (auto& eyes : eyess) {
		for (auto& eye : eyes) {
			eye.Draw(camera);
		}
	}
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {

	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});

	camera.SetScale(1.f);

	bigEye.Emplace()->Init({});
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	bigEye->Update(camera);
}

void SceneTest1::Draw() {
	camera.Calc();

	bigEye->Draw(camera);

	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   drag: mouse", RGBA8_Green, { 0.5f, 1 });
	gLooper.DrawNode(rootNode);
};

#pragma endregion
