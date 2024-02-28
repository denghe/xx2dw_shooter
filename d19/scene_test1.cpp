#include <pch.h>
#include <all.h>

#pragma region Eye

void Eye::Init(BigEye& owner_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_) {
	// todo
}

void Eye::Init(Eye& prev_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_) {
	// todo
}

void Eye::Update(BigEye& owner_) {
	// todo
}

void Eye::Update(Eye& prev_) {
	// todo
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
	pos = {};
	scale = cScale;
	radius = cRadius;
	radians = {};

	// todo: fill children
}

void BigEye::Update(Camera const& camera) {
	// todo: fix drag control
	auto& m = gLooper.mouse;
	if (m.btnStates[0] && !gLooper.mouseEventHandler) {
		auto p = camera.ToLogicPos(m.pos);
		auto d = p - pos;
		if (d.x * d.x + d.y * d.y < radius * radius) {
			pos = p;
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
