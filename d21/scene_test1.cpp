#include <pch.h>
#include <all.h>

#pragma region PhysSceneItem

void PhysSceneItem::PhysAdd() {
	SGCAdd(scene->sgcPhysItems, pos);
}

void PhysSceneItem::PhysUpdate() {
	SGCUpdate(pos);
}

void PhysSceneItem::PhysRemove() {
	SGCRemove();
}

bool PhysSceneItem::PhysExists() {
	return !!_sgc;
}

PhysSceneItem::~PhysSceneItem() {
	SGCRemove();
}

#pragma endregion

#pragma region Staff

xx::Task<> Staff::MainTask() {
	while (true) {
		// follow owner( sync pos )
		if (owner) {
			pos = owner->pos + XY{ 0, 0.1f };
		}

		// try to toward mouse
		auto mPos = scene->camera.ToLogicPos(gEngine->mouse.pos);
		auto d = mPos - pos;
		auto r = std::atan2(d.y, d.x);
		RotateControl::Step(radians, r, cFrameMaxChangeRadians);

		// calc fire pos by current radians
		auto c = std::cos(radians);
		auto s = std::sin(radians);
		firePos = pos + XY{ c, s } *cFireDistance;	// * scale

		// fire
		if (gLooper.mouse.btnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = (float)gLooper.nowSecs + cFireDelaySecs;

			im->Create<Bullet>(xx::WeakFromThis(this));
		}

		co_yield 0;
	}
}

void Staff::Init(ItemManagerBase* im_, xx::Weak<SceneItem> owner_) {
	assert(owner_);
	SceneItemInit(cTypeId, im_);
	owner = std::move(owner_);

	// tasks init
	mainTask = MainTask();
	// ...

	// born init
	//drawOffset = cDrawOffset;
	pos = owner->pos + XY{ 0, 0.1f };
	radians = (float)-M_PI_2;
}

bool Staff::Update() {
	return mainTask.Resume();
}

void Staff::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_staff[cFrameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}

#pragma endregion

#pragma region Bullet

xx::Task<> Bullet::MainTask() {
	// move. in life cycle
	for (int e = gLooper.frameNumber + cLifeNumFrames; gLooper.frameNumber < e;) {
		if (moveTask.Resume()) co_return;
		co_yield 0;
	}

	// move + fadeout
	auto scaleBak = scale;
	auto radiusBak = radius;
	for (float s = 1; s > 0; s -= 1.f / gDesign.fps) {
		scale = scaleBak * s;
		radius = radiusBak * s;
		if (moveTask.Resume()) co_return;
		co_yield 0;
	}
	scale = {};
	radius = 0;
}

xx::Task<> Bullet::MoveTask() {
	XY inc{ std::cos(radians) * cSpeed, std::sin(radians) * cSpeed };
	while (true) {
		pos += inc;
		//// hit check
		//if (auto r = FindNeighborCross(scene->sgcPhysItems, pos, radius)) {
		//	r->Hit(*this, damage);
		//	// find nearest xxx & shoot Bullet 1
		//	if (auto t = FindNearest(scene->sgcPhysItems, gLooper.sgrdd, pos, Bullet1::cAttackRange)) {
		//		im->Create<Bullet1>(owner, pos, radians, xx::WeakFromThis(t), 5);
		//	}
		//	co_return;
		//}
		co_yield 0;
	}
}

void Bullet::Init(ItemManagerBase* im_, xx::Weak<Staff> weapon_) {
	assert(weapon_);
	SceneItemInit(cTypeId, im_);
	owner = weapon_->owner;

	// tasks init
	mainTask = MainTask();
	moveTask = MoveTask();
	// ...

	// get weapon args for born init
	pos = weapon_->firePos;
	radians = weapon_->radians;
	scale = { cScale, cScale };
	// damage = weapon_->damage; ? cDamage ? buff calculate ?
	radius = cRadius;
}

bool Bullet::Update() {
	return mainTask.Resume();
}

void Bullet::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_bullet[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };

	if (scene->isBorderVisible) {
		LineStrip().FillCirclePoints({}, radius, {}, 20, scale * camera.scale)
			.SetPosition(camera.ToGLPos(pos))
			.Draw();
	}
}


#pragma endregion

#pragma region Human

void Human::Init(ItemManagerBase* im_) {
	SceneItemInit(cTypeId, im_);
	scene->human = xx::WeakFromThis(this);

	// tasks init
	mainTask = MainTask();
	idleTask = IdleTask();
	// ...

	// born init
	pos = gCfg.mapCenterPos;
	speed = cSpeed;
	radius = cRadius;
	SetDirection(MoveDirections::Down);

	// weapon init
	im->Create<Staff>(xx::WeakFromThis(this));
}

bool Human::Update() {
	return mainTask.Resume();
}

xx::Task<> Human::IdleTask() {
	while (true) {
		for (scale.y = cIdleScaleYTo; scale.y > cIdleScaleYFrom; scale.y -= cIdleScaleYStep) co_yield 0;
		for (scale.y = cIdleScaleYFrom; scale.y < cIdleScaleYTo; scale.y += cIdleScaleYStep) co_yield 0;
	}
}

XY Human::GetPhysPos() {
	return pos + XY{ 0, -radius / 2 };
}

void Human::SetDirection(MoveDirections d) {
	direction = d;
	int i;
	if (xx::FlagContains(d, MoveDirections::Down)) {
		i = 0;
	} else if (xx::FlagContains(d, MoveDirections::Up)) {
		i = 3;
	} else if (d == MoveDirections::Left) {
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
		scene->camera.SetOriginal(pos);

		co_yield 0;
	}
}

void Human::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_human_1[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos + cDrawOffset);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}

#pragma endregion

#pragma region Monster1

void Monster1::Init(ItemManagerBase* im_, int tracksIndex_, float pointIndex_, float speed_) {
	SceneItemInit(cTypeId, im_);
	scale = { cScale, cScale };
	radius = cRadius;
	speed = speed_;
	tracksIndex = tracksIndex_;
	pointIndex = pointIndex_;
	pos = scene->tracks[tracksIndex_][(int)pointIndex];
	PhysAdd();
	radians = scene->mpc.points[(int)pointIndex].radians;
}

bool Monster1::Update() {
	auto siz = (int)scene->mpc.points.size();
	pointIndex += speed;
	if (pointIndex >= siz) {
		pointIndex -= siz;
	}
	pos = scene->tracks[tracksIndex][(int)pointIndex];
	PhysUpdate();
	radians = scene->mpc.points[(int)pointIndex].radians;
	return false;
}

void Monster1::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
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

	camera.SetMaxFrameSize({ gCfg.maxItemSize, gCfg.maxItemSize });
	camera.SetOriginal(gCfg.mapCenterPos);
	camera.SetScale(1.f);

	sgcPhysItems.Init(gCfg.physNumRows, gCfg.physNumCols, gCfg.physCellSize);
	im.Init(this);

	// create human
	im.Create<Human>();

	// curve points
	std::vector<CurvePoint> cps{ CurvePoint
		{ gCfg.mapCenterPos + XY{ -300, -180} },
		{ gCfg.mapCenterPos + XY{ 300, -180} },
		{ gCfg.mapCenterPos + XY{ 300, 180} },
		{ gCfg.mapCenterPos + XY{ -300, 180} }
	};

	// fill path
	MovePath mp;
	mp.FillCurve(true, cps);

	// path to cache
	mpc.Init(mp, trackStep);

	// cache to tracks
	tracks.Resize(trackCount);
	auto numPoints = (int)mpc.points.size();
	for (auto& track : tracks) {
		track.Resize(numPoints);
	}
	for (int i = 0; i < numPoints; ++i) {
		auto& p = mpc.points[i];
		XY step{
			std::cos(p.radians - gPI / 2) * trackMargin,
			std::sin(p.radians - gPI / 2) * trackMargin
		};
		for (int j = 0; j < trackCount; j++) {
			tracks[j][i] = p.pos + step * j;
		}
	}

	// init main task for create monsters
	tasks.Add([this]()->xx::Task<> {
		while (true) {
			// create some monsters
			if (im.Get<Monster1>().len < 100000) {
				for (int i = 0; i < trackCount; i++) {
					for (int j = 0; j < 10; j++) {
						im.Create<Monster1>(i
							, gLooper.rnd.Next<float>(0, 100)
							, gLooper.rnd.Next<float>(trackBaseSpeed, trackBaseSpeed * 5));
					}
				}
			}
			co_yield 0;
		}
	});
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	im.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	// draw track
	LineStrip ls;
	auto siz = (int)mpc.points.size();
	for (int i = 0; i < siz; i += trackDrawStep) {
		auto& p = mpc.points[i];
		ls.Clear();
		ls.points.emplace_back(camera.ToGLPos(tracks[0][i]));
		ls.points.emplace_back(camera.ToGLPos(tracks[trackCount - 1][i]));
		ls.Draw();
	}

	// draw items ( order by y )
	im.DrawAll(camera);

	auto str = xx::ToString("zoom: Z / X. item count = ", im.GetSize());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });
	gLooper.DrawNode(rootNode);
};

#pragma endregion
