#include <pch.h>
#include <all.h>

#pragma region SnakeBody

void SnakeBody::Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> head_, xx::Weak<SnakeBody> prev_, bool isTail_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask2();
	head = std::move(head);
	prev = std::move(prev_);
	isTail = isTail_;
	// todo: more init
	pos = pos_;
}

xx::Task<> SnakeBody::MainTask() {
	// follow mouse or prev
	while (true) {
		auto hPos = prev ? prev->pos : scene->camera.ToLogicPos(gEngine->mouse.pos);
		auto v = hPos - pos;
		auto distance = std::sqrt(v.x * v.x + v.y * v.y);
		if (auto d = distance - cDistance; d > 0) {
			auto inc = v / distance * std::min(d, cSpeed);
			pos = pos + inc;
		}
		co_yield 0;
	}
}

XY SnakeBody::GenRndPos(float radius, float safeRadius) {
	float len = radius - safeRadius;
	auto r = std::sqrt(gLooper.rnd.Next<float>() * (len / radius) + safeRadius / radius) * radius;
	auto a = gLooper.rnd.Next<float>(gNPI, gPI);
	return pos + XY{ std::cos(a) * r, std::sin(a) * r };
}

xx::Task<> SnakeBody::MainTask2() {
	if (prev) {
		// body logic: follow prev
		do {
			auto hPos = prev->pos;
			auto v = hPos - pos;
			auto distance = std::sqrt(v.x * v.x + v.y * v.y);
			if (auto d = distance - cDistance; d > 0) {
				auto inc = v / distance * std::min(d, cSpeed);
				pos = pos + inc;
			}
			co_yield 0;
		} while (prev);
	}
	// head logic: select random pos & gogogo
	while (true) {
		auto hPos = GenRndPos(300, 50);	// todo: screen range limit ?
		while (true) {
			// todo: simple calc num steps avoid death loop?
			auto v = hPos - pos;
			auto r = std::atan2(v.y, v.x);
			RotateControl::Step(radians, r, cMinRadians);
			XY inc{ std::cos(radians), std::sin(radians) };
			if (v.x * v.x + v.y * v.y > cSpeed * cSpeed) {
				pos = pos + inc * cSpeed;
			} else {
				pos = hPos;
				break;
			}
			co_yield 0;
		}
	}
}

bool SnakeBody::Update() {
	return mainTask.Resume();
}

void SnakeBody::Draw(Camera const& camera) {
#if 0
	RGBA8 c;
	if (prev) {
		if (isTail) c = { 0, 0, 255, 255 };
		else c = { 255, 255, 255, 255 };
	} else {
		c = { 255, 0, 0, 255 };
	}
	auto& q = Quad::DrawOnce(gLooper.frames_creature_1[96]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * cScale;
	q.radians = 0;	// fixed radians
	q.colorplus = 1;
	q.color = c;
#else
	int idx;
	if (prev) {
		if (isTail) idx = 2;
		else idx = 1;
	} else {
		idx = 0;
	}
	auto& q = Quad::DrawOnce(gLooper.frames_snake[idx]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * cScale;
	q.radians = radians;//0;	// fixed radians
	q.colorplus = 1;
	q.color = RGBA8_White;
#endif
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::CreateSnake(XY const& headPos, int len) {
	auto h = im.Create<SnakeBody>(headPos, xx::Weak<SnakeBody>{}, xx::Weak<SnakeBody>{}, false);
	h->radians = -gPI / 2;
	auto o = h;
	for (int i = 0; i < len; i++) {
		o = im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance }, h, o);
	}
	im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance }, h, o, true);
}

void SceneTest1::Init() {
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Label>()->Init(1, gDesign.xy8m, { 1,1 }, gDesign.xy8a, RGBA8_White
		, "zoom: Z / X  create snake: mouse click");

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"clear", [&]() {
		im.Clear();
		});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m, gDesign.xy4a, gLooper.s9cfg_btn, U"+100 snake", [&]() {
		for (int i = 0; i < 100; i++) {
			CreateSnake(camera.ToLogicPos({}), gLooper.rnd.Next<int>(10, 30));
		}
		});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});

	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	camera.SetScale(1.f);

	im.Init(this);
}

void SceneTest1::Update() {
	camera.Calc();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// snake gen control
	auto& m = gLooper.mouse;
	if (m.btnStates[0]) {
		CreateSnake(camera.ToLogicPos(m.pos), gLooper.rnd.Next<int>(10, 30));
	}

	im.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	im.DrawAll(camera);

	gLooper.DrawNode(rootNode);
};

#pragma endregion
