﻿#include <pch.h>
#include <all.h>

#pragma region SnakeBody

void SnakeBody::Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> prev_, bool isTail_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask();
	prev = std::move(prev_);
	isTail = isTail_;
	// todo: more init
	pos = pos_;
}

xx::Task<> SnakeBody::MainTask() {
LabBegin:
	for (int e = gLooper.frameNumber + int(cStepMoveDuration / gDesign.frameDelay); gLooper.frameNumber < e;) {
		auto hPos = prev ? prev->pos : scene->camera.ToLogicPos(gEngine->mouse.pos);
		auto d = hPos - pos;
		auto dd = d.x * d.x + d.y * d.y;
		// todo: do not overlap ( cDistance )
		if (dd > cSpeed * cSpeed) {						// avoid NAN issue
			auto inc = d / std::sqrt(dd);			// normalize
			pos = pos + inc * cSpeed;
		} else {
			pos = hPos;
		}
		co_yield 0;
	}
	goto LabBegin;
}

bool SnakeBody::Update() {
	return mainTask.Resume();
}

void SnakeBody::Draw(Camera const& camera) {
	xx::Ref<Frame> f;
	if (prev) {
		if (isTail) f = gLooper.frame_no;
		else f = gLooper.frames_mine[0];
	} else {
		f = gLooper.frame_yes;
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

	// create snake
	auto o = im.Create<SnakeBody>(gLooper.windowSize_2, xx::Weak<SnakeBody>{});
	for (size_t i = 0; i < 20; i++) {
		o = im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance.from }, o);
	}
	im.Create<SnakeBody>(o->pos + XY{ 0, o->cDistance.from }, o, true);
}

void SceneTest1::Update() {
	im.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	im.DrawAll(camera);
};

#pragma endregion
