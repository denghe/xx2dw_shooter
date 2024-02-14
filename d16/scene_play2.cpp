#include <pch.h>
#include <all.h>

// use coroutine. do not support ECS

/************************************************************************************************************/
// ScenePlay2

void ScenePlay2::Init() {
	// init gui
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// init camera
	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	camera.SetScale(4);

	// init im
	im.Init();
	im.Create<Slime>();
	im.Create<Human>();
}

void ScenePlay2::Update() {
	im.Update();
	im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
		Sort(items);
	});
}

void ScenePlay2::Draw() {
	camera.Calc();

	// ...

	// items
	im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
		for (auto& o : items) {
			if (camera.InArea(o->pos)) {
				iys.Emplace(o.pointer, o->pos.y);
			}
		}
	});
	Sort(iys);
	for (auto& iy : iys) {
		iy.item->Draw(camera);
	}
	iys.Clear();

	// ...

	// ui
	gLooper.DrawNode(rootNode);
};


/************************************************************************************************************/
// Human

void Human::Init(ItemManagerBase* im_) {
	ItemInit(cTypeId, im_);

	// tasks init
	mainTask = MainTask();
	idleTask = IdleTask();
	// ...

	// born init
	pos = gLooper.windowSize_2.MakeAdd(0, 0);
	speed = cSpeed;
	SetDirection(MoveDirections::Down);
}

int Human::UpdateCore() {
	return !mainTask.Resume();
}

xx::Task<> Human::IdleTask() {
	while (true) {
		for (scale.y = cIdleScaleYTo; scale.y > cIdleScaleYFrom; scale.y -= cIdleScaleYStep) co_yield 0;
		for (scale.y = cIdleScaleYFrom; scale.y < cIdleScaleYTo; scale.y += cIdleScaleYStep) co_yield 0;
	}
}

void Human::SetDirection(MoveDirections d) {
	direction = d;
	int i;
	if (xx::FlagContains(d, MoveDirections::Down)) {
		i = 0;
	} else if (xx::FlagContains(d, MoveDirections::Up)) {
		i = 3;
	} else if (d == MoveDirections::Left){
		i = 1;
	} else {
		i = 2;
	}
	frameIndexFrom = cFrameIndexRanges[i];
	frameIndexTo = cFrameIndexRanges[i + 1];
	frameIndex = frameIndexFrom;
}

xx::Task<> Human::MainTask() {
	while (true) {
		// keyboard move control
		if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
			pos += r->second * speed;
			if (direction != r->first) {
				SetDirection(r->first);
			}
			FrameControl::Forward(frameIndex, cFrameInc * speed, frameIndexFrom, frameIndexTo);
		} else {
			idleTask();
		}

		co_yield 0;
	}
}

void Human::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_human_1[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}


/************************************************************************************************************/
// Slime

void Slime::Init(ItemManagerBase* im_) {
	ItemInit(cTypeId, im_);
	mainTask = MainTask();
	pos = gLooper.windowSize_2.MakeAdd(100, 0);
}

int Slime::UpdateCore() {
	return !mainTask.Resume();
}

xx::Task<> Slime::MainTask() {
	while (true) {
		co_yield 0;
	}
}

void Slime::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_creature_slime[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}
