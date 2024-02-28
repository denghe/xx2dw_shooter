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

	// calc radians & pos
	Update();
}

void Eye::Update() {
	radians = std::atan2(pos.y - prev->pos.y, pos.x - prev->pos.x);
	RotateControl::Limit(radians, prev->radians + cRadiansRange.from, prev->radians + cRadiansRange.to);
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
	static constexpr int numEyess{ 30 }, numEyes{ 30 };
	static constexpr float step{ g2PI / numEyess };
	eyess.Resize(numEyess);
	for (int i = 0; i < numEyess; i++) {
		auto& eyes = eyess[i];
		eyes.Resize(numEyes);
		auto r = step * i;
		eyes[0].Init(*this, {r, r}, { radius, radius });
		for (int j = 1; j < numEyes; j++) {
			eyes[j].Init(eyes[j - 1], {-0.3f, 0.3f}, { Eye::cRadius / 2, Eye::cRadius / 2 });
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
