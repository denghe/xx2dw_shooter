#include <pch.h>
#include <all.h>

#pragma region Scene[Phys]Item

void SceneItem::SceneItemInit(int typeId_, ItemManagerBase* im_) {
	ItemInit(typeId_, im_);
	scene = (ScenePlay2*)im_->userData;
}

void ScenePhysItem::PhysAdd() {
	SGCAdd(scene->sgcPhysItems, pos.As<int>());
}

void ScenePhysItem::PhysUpdate() {
	SGCUpdate(pos.As<int>());
}

void ScenePhysItem::PhysRemove() {
	SGCRemove();
}

ScenePhysItem::~ScenePhysItem() {
	SGCRemove();
}

#pragma endregion

#pragma region BornMask

void BornMask::Init(std::function<void()> onDispose_, XY const& pos_, XY const& anchor_, float scale_) {
	onDispose = std::move(onDispose_);
	pos = pos_;
	anchor = anchor_;
	scale = scale_;
}

bool BornMask::Update() {
	lineNumber = UpdateCore();
	return lineNumber == 0;
}

int BornMask::UpdateCore() {
	COR_BEGIN
	for (i = 0; i < 5; i++) {
		visible = true;
		for (e = gLooper.frameNumber + int(cVisibleTimeSpan / gDesign.frameDelay); gLooper.frameNumber < e;) {
			COR_YIELD
		}
		visible = false;
		for (e = gLooper.frameNumber + int(cHideTimeSpan / gDesign.frameDelay); gLooper.frameNumber < e;) {
			COR_YIELD
		}
	}
	onDispose();
	COR_END
}

void BornMask::Draw(Camera const& camera) {
	if (!visible || !camera.InArea(pos)) return;
	auto& q = Quad::DrawOnce(gLooper.frame_no);
	q.pos = camera.ToGLPos(pos);
	q.anchor = anchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}


void BornMaskManager::Init(size_t cap) {
	os.Reserve(cap);
}

void BornMaskManager::Add(std::function<void()> onDispose_, XY const& pos_, XY const& anchor_, float scale_) {
	os.Emplace().Init(std::move(onDispose_), pos_, anchor_, scale_);
}

bool BornMaskManager::Update() {
	os.Foreach([](BornMask& o)->bool {
		return o.Update();
	});
	return os.Empty();
}

void BornMaskManager::Draw(Camera const& camera) {
	os.Foreach([&](BornMask& o)->void {
		o.Draw(camera);
	});
}

#pragma endregion

#pragma region Weapon

xx::Task<> Weapon::MainTask() {
	while (true) {
		// follow owner( sync pos )
		if (owner) {
			pos = owner->pos + XY{ 0, 0.1f };
		}

		// finger to mouse
		auto mPos = scene->camera.ToLogicPos(gEngine->mouse.pos);
		auto d = mPos - pos;
		auto r = std::atan2(d.y, d.x);
		RotateControl::Step(radians, r, cFrameMaxChangeRadians);

		// todo: optimize ? user normalize?
		auto c = std::cos(radians);
		auto s = std::sin(radians);
		firePos = pos + XY{ c, s } * cFireDistance;	// * scale

		// fire?
		if (gLooper.mouse.btnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = (float)gLooper.nowSecs + cFireDelaySecs;
			//auto r = -radians;
			//auto c = std::cos(r);
			//auto s = -std::sin(r);
			//xx::CoutN(c, "  ", s, "  ", firePos);
			im->Create<Bullet>(xx::WeakFromThis(this));
			scene->audio.Play(gLooper.soundDatas[3]);
		}

		co_yield 0;
	}
}

void Weapon::Init(ItemManagerBase* im_, xx::Weak<SceneItem> owner_) {
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

int Weapon::UpdateCore() {
	return !mainTask.Resume();
}

void Weapon::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_staff[cFrameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };

	if (scene->isBorderVisible) {
		LineStrip().FillCirclePoints({}, 5, -radians, 20, scale * camera.scale)
			.SetPosition(camera.ToGLPos(pos))
			.Draw()
			.SetPosition(camera.ToGLPos(firePos))
			.Draw();
	}
}

#pragma endregion

#pragma region Bullet

xx::Task<> Bullet::MainTask() {
	// move. in life cycle
	for (int e = gLooper.frameNumber + cLifeNumFrames; gLooper.frameNumber < e;) {
		moveTask.Resume();
		// todo: hit check
		co_yield 0;
	}

	// move + fadeout
	auto scaleBak = scale;
	auto radiusBak = radius;
	for (float s = 1; s > 0; s -= 1.f / gDesign.fps) {
		scale = scaleBak * s;
		radius = radiusBak * s;
		moveTask.Resume();
		// todo: hit check
		co_yield 0;
	}
	scale = {};
	radius = 0;
}

xx::Task<> Bullet::MoveTask() {
	XY inc{ std::cos(radians) * cSpeedByFrame, std::sin(radians) * cSpeedByFrame };
	while (true) {
		pos += inc;
		FrameControl::Forward(frameIndex, cFrameInc, cFrameMaxIndex);
		co_yield 0;
	}
}

void Bullet::Init(ItemManagerBase* im_, xx::Weak<Weapon> weapon_) {
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
	// damage = weapon_->damage; ? cDamage ? buff calculate ?
	radius = cRadius;
}

int Bullet::UpdateCore() {
	return !mainTask.Resume();
}

void Bullet::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_mine[(int)frameIndex]);
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
	pos = gMapCfg.mapCenterPos;
	speed = cSpeed;
	radius = cRadius;
	SetDirection(MoveDirections::Down);

	// weapon init
	im->Create<Weapon>(xx::WeakFromThis(this));
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
		scene->camera.SetOriginal(pos);

		co_yield 0;
	}
}

void Human::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_human_1[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos + cDrawOffset);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}

#pragma endregion

#pragma region Slime

void Slime::Init(ItemManagerBase* im_, XY const& pos_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask();
	animTask = AnimTask();
	moveTask = MoveTask();
	pos = pos_;
	radius = cRadius;
	speed = cSpeed;
	PhysAdd();
}

int Slime::UpdateCore() {
	if (!freeze) {
		animTask.Resume();
	}
	return !mainTask.Resume();
}

xx::Task<> Slime::AnimTask() {
	while (true) {
		FrameControl::Forward(frameIndex, cFrameInc * speed, cFrameMaxIndex);
		co_yield 0;
	}
}

xx::Task<> Slime::MoveTask() {
	while (true) {
		auto hPos = scene->human->GetPhysPos();
		for (int e = gLooper.frameNumber + int(cStepMoveDuration / gDesign.frameDelay); gLooper.frameNumber < e;) {
			auto d = hPos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (dd > cSpeed * cSpeed) {						// avoid NAN issue
				auto inc = d / std::sqrt(dd);			// normalize
				pos = pos + inc * cSpeed;
			} else {
				pos = hPos;
			}
			PhysUpdate();
			co_yield 0;
		}
	}
}

xx::Task<> Slime::MainTask() {
	// idle a while
	for (int e = gLooper.frameNumber + cBornIdleDelayFrames; gLooper.frameNumber < e;) {
		co_yield 0;
	}

	// track player in life cycle
	for (int e = gLooper.frameNumber + cLifeNumFrames; gLooper.frameNumber < e;) {
		if (scene->human) {
			moveTask.Resume();
		}
		co_yield 0;
	}

	// fadeout
	auto scaleBak = scale;
	auto radiusBak = radius;
	for (float s = 1; s > 0; s -= 1.f / gDesign.fps) {
		scale = scaleBak * s;
		radius = radiusBak * s;
		co_yield 0;
	}
	scale = {};
	radius = 0;
}

void Slime::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_creature_slime[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos + cDrawOffset);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 255 };
}

#pragma endregion

#pragma region ScenePlay2

void ScenePlay2::Init() {
	// init gui					// todo: fix gui pos when window resize ?
	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", []() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m, gDesign.xy8a, gLooper.s9cfg_btn, U"Show/Hide Physics Border", [this]() {
		this->isBorderVisible = !this->isBorderVisible;
	});

	// init camera
	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	camera.SetScale(1);

	// init objs
	sgcPhysItems.Init(gMapCfg.physNumRows, gMapCfg.physNumCols, gMapCfg.physCellSize);
	im.Init(this);
	im.Create<Human>();
}

//#define ENABLE_SORT_WHEN_UPDATE

void ScenePlay2::Update() {
	audio.Update();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// simulate room master logic
	for (int i = 0; i < 1; i++) {
		MakeSlime();
	}

	// update objs
	bmm.Update();
	im.Update();
#ifdef ENABLE_SORT_WHEN_UPDATE
	im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
		Sort(items);
	});
#endif
}

void ScenePlay2::Draw() {
	camera.Calc();

	// ...
	// born masks
	bmm.Draw(camera);

	// items
#ifdef ENABLE_SORT_WHEN_UPDATE
	im.ForeachAllItems([&]<typename T>(xx::Listi32<xx::Shared<T>>&items) {
		for (auto& o : items) {
			if (camera.InArea(o->pos)) {
				iys.Emplace(o.pointer, o->pos.y);
			}
		}
	});
#else
	if (human) {
		iys.Emplace(human.GetPointer(), human->posY);
	}

	im.ForeachItems<Weapon, Bullet>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items) {
		for (auto& o : items) {
			if (camera.InArea(o->pos/* + o->drawOffset*/)) {
				iys.Emplace(o, o->posY);
			}
		}
	});

	// fill phys items to iys order by row & cut by cell pos
	int32_t rowFrom, rowTo, colFrom, colTo; 
	camera.FillRowColIdxRange(gMapCfg.physNumRows, gMapCfg.physNumCols, gMapCfg.physCellSize,
		rowFrom, rowTo, colFrom, colTo);
	for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
		for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
			auto idx = sgcPhysItems.CrIdxToCellIdx({ colIdx, rowIdx });
			sgcPhysItems.ForeachWithoutBreak(idx, [&](ScenePhysItem* o) {
				iys.Emplace(o, o->posY);
			});
		}
	}
#endif
	ItemY::Sort(iys);
	for (auto& iy : iys) {
		iy.item->Draw(camera);
	}
	iys.Clear();

	// ...

	// draw phys circle for debug
	if (isBorderVisible) {
		for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
			for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
				auto idx = sgcPhysItems.CrIdxToCellIdx({ colIdx, rowIdx });
				sgcPhysItems.ForeachWithoutBreak(idx, [&](ScenePhysItem* o) {
					LineStrip()
						.FillCirclePoints({}, o->radius, o->radians, 20, XY::Make(camera.scale * o->scale.x))
						.SetPosition(camera.ToGLPos(o->pos))
						.Draw();
					});
			}
		}
		if (human) {
			LineStrip()
				.FillCirclePoints({}, human->radius, human->radians, 20, XY::Make(camera.scale* human->scale.x))
				.SetPosition(camera.ToGLPos(human->pos))
				.Draw();
		}
	}

	// todo: draw map border for debug ?

	// ui
	gLooper.DrawNode(rootNode);
};

void ScenePlay2::MakeSlime() {
	if (!human) return;
	XY pos;

#if 1
	static constexpr float pi{ (float)M_PI }, npi{ -pi }, pi2{ pi * 2 };
	// circle gen , outside the human's safe range
	static constexpr float radius = 400, safeRadius = 300, len = radius - safeRadius;
	auto r = std::sqrt(gLooper.rnd.Next<float>() * (len / radius) + safeRadius / radius ) * radius;
	auto a = gLooper.rnd.Next<float>(npi, pi);
	pos.x = std::cos(a) * r;
	pos.y = std::sin(a) * r;
	pos += human->GetPhysPos();
#else
	// full map random gen
	static constexpr XY ws{ gMapCfg.mapSize.x - 64, gMapCfg.mapSize.y - 64 };
	pos.x = gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.x - gMapCfg.physCellSize);
	pos.y = gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.y - gMapCfg.physCellSize);
#endif

	bmm.Add([this, pos] {
		this->im.Create<Slime>(pos);
	}, pos, Slime::cBornMaskAnchor, Slime::cBornMaskScale);
}

#pragma endregion
