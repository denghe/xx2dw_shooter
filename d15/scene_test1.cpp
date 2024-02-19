#include <pch.h>
#include <all.h>

#pragma region SnakeBody

void SnakeBody::Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> head_, xx::Weak<SnakeBody> prev_, bool isTail_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask();
	head = std::move(head);
	prev = std::move(prev_);
	isTail = isTail_;
	// todo: more init
	pos = pos_;
}

xx::Task<> SnakeBody::MainTask() {
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

bool SnakeBody::Update() {
	return mainTask.Resume();
}

void SnakeBody::Draw(Camera const& camera) {
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
	q.radians = radians;
	q.colorplus = 1;
	q.color = c;
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	camera.SetScale(4.f);

	im.Init(this);

	// create snake
	auto h = im.Create<SnakeBody>(gLooper.windowSize_2, xx::Weak<SnakeBody>{}, xx::Weak<SnakeBody>{}, false);
	auto o = h;
	for (size_t i = 0; i < 20; i++) {
		o = im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance }, h, o);
	}
	im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance }, h, o, true);
}

void SceneTest1::Update() {
	im.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	im.DrawAll(camera);
};

#pragma endregion
