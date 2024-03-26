#include <pch.h>
#include <all.h>

#pragma region Eye

void Eye::Init(EyeBase& prev_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_) {
	// init args
	cRadiansRange = cRadiansRange_;
	cDistanceRange = cDistanceRange_;
	scale = cScale;
	radius = cRadius;
	radians = prev_.radians;
	pos = prev_.pos;

	// calc radians & pos
	Update(prev_);
}

void Eye::Update(EyeBase& prev_) {
	radians = std::atan2(pos.y - prev_.pos.y, pos.x - prev_.pos.x);
	RotateControl::Limit(radians, prev_.radians + cRadiansRange.from, prev_.radians + cRadiansRange.to);
	float d;
	if (cDistanceRange.from != cDistanceRange.to) {
		d = Calc::DistanceLimit(Calc::Distance(pos, prev_.pos), cDistanceRange.from, cDistanceRange.to);
	} else {
		d = cDistanceRange.from;
	}
	pos.x = prev_.pos.x + std::cos(radians) * d;
	pos.y = prev_.pos.y + std::sin(radians) * d;
}

void Eye::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = camera.scale * scale;
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
			eyes[j].Init(eyes[j - 1], { -0.1f, 0.1f }
			, { Eye::cRadius / 2, Eye::cRadius / 1.5f });
		}
	}
}

void BigEye::Update(Camera const& camera) {
	auto& m = gLooper.mouse;
	auto p = camera.ToLogicPos(m.pos);
	auto v = p - pos;
	if (m.btnStates[0] && !gLooper.mouseEventHandler) {
		// follow mouse
		auto distance = std::sqrt(v.x * v.x + v.y * v.y);
		if (auto d = distance - cSpeed; d > 0) {
			auto inc = v / distance * cSpeed;
			pos = pos + inc;
		} else {
			pos = p;
		}
	} else {
		// rotate to mouse
		auto mr = std::atan2(v.y, v.x);
		radians = RotateControl::LerpAngleByFixed(mr, radians, gPI / 10);
	}

	for (auto& eyes : eyess) {
		eyes[0].Update(*this);
		for (int i = 1, e = eyes.len; i < e; ++i) {
			eyes[i].Update(eyes[i - 1]);
		}
	}
}

void BigEye::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = camera.scale * scale;
	q.radians = radians;
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

	for (size_t i = 0; i < 100; i++) {
		bigEyes.Emplace().Emplace()->Init({});
	}
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	for (auto& o : bigEyes) {
		o->Update(camera);
	}
}

void SceneTest1::Draw() {
	camera.Calc();

	// todo: sort by y?
	for (auto& o : bigEyes) {
		o->Draw(camera);
	}

	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   drag: mouse", RGBA8_Green, { 0.5f, 1 });
	gLooper.DrawNode(rootNode);
};

#pragma endregion
