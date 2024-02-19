#include <pch.h>
#include <all.h>

#pragma region SnakeBody

void SnakeBody::Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> prev_, xx::Weak<SnakeBody> next_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask();
	prev = std::move(prev_);
	next = std::move(next_);
	// todo: more init
	pos = pos_;
}

xx::Task<> SnakeBody::MainTask() {
	while (prev) {
		// todo: follow
		co_yield 0;
	}
	while (true) {
		// todo: is head: random move
		co_yield 0;
	}
}

bool SnakeBody::Update() {
	return mainTask.Resume();
}

void SnakeBody::Draw(Camera const& camera) {
	xx::Ref<Frame> f;
	if (prev) {
		if (next);	// f = tail
		else;	// f = body
	} else {
		// f = head
	}
	auto& q = Quad::DrawOnce(f);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * cScale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	camera.SetScale(2.f);

	im.Init(this);

	// todo: create snake
	im.Create<SnakeBody>(gLooper.windowSize_2, xx::Weak<SnakeBody>{}, xx::Weak<SnakeBody>{});
}

void SceneTest1::Update() {
	im.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	im.DrawAll(camera);
};

#pragma endregion
