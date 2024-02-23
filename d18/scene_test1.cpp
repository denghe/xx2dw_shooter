#include <pch.h>
#include <all.h>

#pragma region TailCell

void TailCell::Calc() {
	assert(tail);
	assert(prev);
	radians = tail->radians + tail->curvature * radiansRange;
	pos.x = prev->pos.x + std::cos(radians) * distance;
	pos.y = prev->pos.y + std::sin(radians) * distance;
}

void TailCell::Init(xx::Weak<Tail> tail_, xx::Weak<TailBase> prev_, float distance_, float radiansRange_) {
	tail = std::move(tail_);
	prev = std::move(prev_);
	distance = distance_ * tail->scale;
	radiansRange = radiansRange_;
	radius = cRadius * tail->scale;
	Calc();
}

void TailCell::Update() {
	Calc();
}

void TailCell::Draw(Camera const& camera) {
	assert(tail);
	auto& q = Quad::DrawOnce(gLooper.frames_mine[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = {0.5f, 0.5f};
	q.scale = XY::Make(camera.scale * tail->scale);
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
}

#pragma endregion

#pragma region Tail

void Tail::Calc() {
	radians = owner->radians + ownerOffsetRadians;
	pos = owner->pos + XY{ std::cos(radians), std::sin(radians) } * owner->radius;
}

Tail& Tail::Init(XY const& pos_, float radians_, int len_, float scale_) {
	pos = pos_;
	radians = radians_;
	scale = scale_;
	InitCore(len_);
	return *this;
}

Tail& Tail::Init(xx::Weak<TailBase> owner_, float ownerOffsetRadians_, int len_, float scale_) {
	owner = std::move(owner_);
	ownerOffsetRadians = ownerOffsetRadians_;
	scale = scale_;
	Calc();
	InitCore(len_);
	return *this;
}

void Tail::InitCore(int len_) {
	mainTask = MainTask();
	auto w = xx::WeakFromThis(this);
	xx::Weak<TailBase> p = w;
	auto r = gPI / len_;
	for (int i = 0; i < len_; i++) {
		auto& o = cells.Emplace().Emplace();
		o->Init(w, p, 5, (r * i) * (r * i));
		p = o;
	}
}

xx::Task<> Tail::MainTask() {
	for (curvature = 0.f; curvature < 1.f; curvature += cCurvatureStep) co_yield 0;
	while (true) {
		for (; curvature > -1.f; curvature -= cCurvatureStep) co_yield 0;
		for (; curvature < 1.f; curvature += cCurvatureStep) co_yield 0;
	}
}

void Tail::Update() {
	if (owner) {
		Calc();
	}
	mainTask.Resume();
	for (auto& o : cells) {
		o->Update();
	}
}

void Tail::Draw(Camera const& camera) {
	for (int i = cells.len - 1; i >= 0; --i) {
		cells[i]->Draw(camera);
	}
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ 32, 32 });
	camera.SetOriginal({});
	camera.SetScale(1.f);

	static constexpr int len1{ 16 }, len2{ 8 }, len3{ 4 };
	static constexpr float scale1{ 32.f }, scale2{ 4.f }, scale3{ 0.5f };

	tails.Emplace().Emplace()->Init(XY{ 0, -300}, gPI / 2, len1, scale1)
		.ForeachCell([&](xx::Shared<TailCell>& c) {
		tails.Emplace().Emplace()->Init(c, gPI / 2, len2, scale2)
			.ForeachCell([&](xx::Shared<TailCell>& c) {
			tails.Emplace().Emplace()->Init(c, gPI / 2, len3, scale3);
			tails.Emplace().Emplace()->Init(c, -gPI / 2, len3, scale3);
		});
		tails.Emplace().Emplace()->Init(c, -gPI / 2, len2, scale2)
			.ForeachCell([&](xx::Shared<TailCell>& c) {
			tails.Emplace().Emplace()->Init(c, gPI / 2, len3, scale3);
			tails.Emplace().Emplace()->Init(c, -gPI / 2, len3, scale3);
		});
	});

	//static constexpr float step = g2PI / 20;
	//static constexpr float distance = 50;
	//static constexpr int len = 20;
	//for (float r = gNPI; r < gPI; r+= step) {
	//	XY pos{ std::cos(r) * distance,std::sin(r) * distance };
	//	tails.Emplace().Emplace()->Init(pos, r, len);
	//}
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	for (auto& o : tails) {
		o->Update();
	}
}

void SceneTest1::Draw() {
	camera.Calc();

	for (auto& o : tails) {
		o->Draw(camera);
	}
};

#pragma endregion
