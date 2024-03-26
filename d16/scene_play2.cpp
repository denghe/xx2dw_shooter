#include <pch.h>
#include <all.h>

#pragma region Scene[Phys]Item

void SceneItem::SceneItemInit(int typeId_, ItemManagerBase* im_) {
	ItemInit(typeId_, im_);
	scene = (ScenePlay2*)im_->userData;
}

void ScenePhysItem::PhysAdd() {
	SGCAdd(scene->sgcPhysItems, pos);
}

void ScenePhysItem::PhysUpdate() {
	SGCUpdate(pos);
}

void ScenePhysItem::PhysRemove() {
	SGCRemove();
}

bool ScenePhysItem::InPhys() {
	return !!_sgc;
}

ScenePhysItem::~ScenePhysItem() {
	SGCRemove();
}

bool ScenePhysItem::Hit(SceneItem& attacker, int damage) {
	SGCRemove();
	return true;
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
	q.scale = camera.scale * scale;
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
		firePos = pos + XY{ c, s } * cFireDistance;	// * scale

		// fire
		if (gLooper.mouse.btnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = (float)gLooper.nowSecs + cFireDelaySecs;

			gEngine->audio.Play(gLooper.soundDatas[0]);

#if 1
			// multi line fire
			auto radiansBak = radians;
			static constexpr float step = g2PI / 20;
			for (float a = gNPI; a < gPI; a += step) {
				radians = radiansBak + a;
				im->Create<Bullet>(xx::WeakFromThis(this));
			}
			radians = radiansBak;
#else
			im->Create<Bullet>(xx::WeakFromThis(this));
#endif
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

int Staff::UpdateCore() {
	return !mainTask.Resume();
}

void Staff::Draw(Camera const& camera) {
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
		scene->bulletTails.Emplace().Init(*this);
		// hit check
		if (auto r = FindNeighborCross(scene->sgcPhysItems, pos, radius)) {
			r->Hit(*this, damage);
			// find nearest xxx & shoot Bullet 1
			if (auto t = FindNearest(scene->sgcPhysItems, gLooper.sgrdd, pos, Bullet1::cAttackRange)) {
				im->Create<Bullet1>( owner, pos, radians, xx::WeakFromThis(t), 5);
			}
			co_return;
		}
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

int Bullet::UpdateCore() {
	return !mainTask.Resume();
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

#pragma region BulletTail

void BulletTail::Init(Bullet& bullet_) {
	lifeEndFrameNumber = gLooper.frameNumber + cLifeNumFrames;
	// copy args from bullet_
	frameIndex = bullet_.frameIndex;
	pos = bullet_.pos;
	flipX = bullet_.flipX;
	scale = bullet_.scale;
	radians = bullet_.radians;
}

int BulletTail::UpdateCore() {
	return gLooper.frameNumber > lifeEndFrameNumber;
}

void BulletTail::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_bullet[6]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = Bullet::cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = { 255, 255, 255, 88 };
}

#pragma endregion

#pragma region Bullet1

xx::Task<> Bullet1::MainTask() {
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

void Bullet1::MoveToTarget() {
	auto d = target->pos - pos;
	auto dd = d.x * d.x + d.y * d.y;
	if (dd > cSpeed * cSpeed) {
		inc = d / std::sqrt(dd) * cSpeed;
		pos = pos + inc;
	} else {
		pos = target->pos;
	}
	radians = std::atan2(d.y, d.x);
}

xx::Task<> Bullet1::MoveTask() {
	// follow
	while (target) {
		MoveToTarget();
		// hit check
		if (auto r = FindNeighborCross(scene->sgcPhysItems, pos, radius)) {
			r->Hit(*this, damage);
			if (life > 0) {
				// find nearest xxx & shoot Bullet 1
				if (auto t = FindNearest(scene->sgcPhysItems, gLooper.sgrdd, pos, cAttackRange)) {
					im->Create<Bullet1>(owner, pos, radians, xx::WeakFromThis(t), life - 1);
				}
			}
			co_return;
		}
		co_yield 0;
	}
	// direct fly
	if (inc.x * inc.x + inc.y + inc.y < cSpeed * cSpeed) {
		inc.x = std::cos(radians) * cSpeed;
		inc.y = std::sin(radians) * cSpeed;
	}
	while (true) {
		pos += inc;
		// hit check
		if (auto r = FindNeighborCross(scene->sgcPhysItems, pos, radius)) {
			r->Hit(*this, damage);
			co_return;
		}
		co_yield 0;
	}
}

void Bullet1::Init(ItemManagerBase* im_, xx::Weak<SceneItem> owner_, XY const& pos_, float radians_, xx::Weak<ScenePhysItem> target_, int life_) {
	SceneItemInit(cTypeId, im_);
	owner = std::move(owner_);

	// tasks init
	mainTask = MainTask();
	moveTask = MoveTask();
	// ...

	// get weapon args for born init
	pos = pos_;
	radians = radians_;
	scale = { cScale, cScale };
	target = std::move(target_);
	life = life_;
	inc = {};
	// damage = weapon_->damage; ? cDamage ? buff calculate ?
	radius = cRadius;
	if (target) {
		MoveToTarget();
	}
}

int Bullet1::UpdateCore() {
	return !mainTask.Resume();
}

void Bullet1::Draw(Camera const& camera) {
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

#pragma region Book

void Book::CalcPos() {
	assert(center);
	pos = center->pos + XY{ std::cos(radians), std::sin(radians) } *distance;
}

xx::Task<> Book::MainTask() {
	float radiansBak = radians;
	while (center) {
		for (float a = gNPI; a < gPI; a += cRadiansStep) {
			radians = radiansBak + a * aMut;
			CalcPos();
			// hit check
			FindNeighborsCross(scene->sgcPhysItems, pos, radius, [&](auto p) {
				p->Hit(*this, damage);
			});
			co_yield 0;
		}
	}
}

void Book::Init(ItemManagerBase* im_, xx::Weak<SceneItem> owner_, xx::Weak<SceneItem> center_, float radians_, float radius_, int frameIndex_, float aMut_) {
	assert(owner_);
	SceneItemInit(cTypeId, im_);
	owner = std::move(owner_);
	center = std::move(center_);
	frameIndex = (float)frameIndex_;
	aMut = aMut_;

	// tasks init
	mainTask = MainTask();
	// ...

	// born init
	radius = radius_;
	scale = XY{ cScale, cScale } * (radius_ / cRadius);
	distance = cDistance * (radius_ / cRadius);
	radians = radians_;
	CalcPos();
	// damage = weapon_->damage; ? cDamage ? buff calculate ?
}

int Book::UpdateCore() {
	return !mainTask.Resume();
}

void Book::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_icon_book_1[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY{ flipX ? -camera.scale : camera.scale, camera.scale } *scale;
	q.radians = 0;// radians;
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
	im->Create<Staff>(xx::WeakFromThis(this));

	static constexpr float step = g2PI / 5;
	for (float a = gNPI; a < gPI; a += step) {
		auto o = im->Create<Book>(xx::WeakFromThis(this), xx::WeakFromThis(this), a, Book::cRadius, 14, 1.f);
		for (float b = gNPI; b < gPI; b += step) {
			im->Create<Book>(xx::WeakFromThis(this), o.ToWeak(), b, Book::cRadius / 2, 21, -2.f);
		}
	}
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

bool Slime::Hit(SceneItem& attacker, int damage) {
	// todo: hp check?
	// todo: play effect? add exp to player? ....
	scene->enm.Add(pos, pos - attacker.pos, { 255,0,0,255 }, hp);
	return ScenePhysItem::Hit(attacker, damage);
}

void Slime::Init(ItemManagerBase* im_, XY const& pos_) {
	SceneItemInit(cTypeId, im_);
	mainTask = MainTask();
	animTask = AnimTask();
	moveTask = MoveTask();
	radius = cRadius;
	speed = cSpeed;
	hp = gLooper.rnd.Next<int>(200, 1000);
	pos = pos_;
	PhysAdd();
}

int Slime::UpdateCore() {
	if (!InPhys()) return 0;	// removed from phys: suside
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
	camera.SetScale(2.f);

	// init objs
	sgcPhysItems.Init(gMapCfg.physNumRows, gMapCfg.physNumCols, gMapCfg.physCellSize);
	im.Init(this);
	im.Create<Human>();

	// init main logic
	tasks.Add(MainTask());
}

xx::Task<> ScenePlay2::MainTask() {

	while (true) {
#if 1
		for (int i = 0; i < 30; i++) {
			MakeSlime1(300, 100);
		}
#else
		co_await gLooper.AsyncSleep(1);
		if (human) {
			XY offset{ gLooper.rnd.Next<float>(50, 150), gLooper.rnd.Next<float>(-50, 50) };
			MakeSlime3(human->pos + offset);
		}
#endif
		co_yield 0;
	}
}

void ScenePlay2::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// update objs
	bmm.Update();
	enm.Update();
	im.Update();
}

void ScenePlay2::Draw() {
	camera.Calc();

	// ...
	// draw other effects?

	// born masks
	bmm.Draw(camera);

	// items
	if (human) {
		iys.Emplace(human.GetPointer(), human->posY);
	}

	im.ForeachItems<Staff, Bullet, Bullet1, Book>([&]<typename T>(xx::Listi32<xx::Shared<T>>& items) {
		for (auto& o : items) {
			if (camera.InArea(o->pos/* + o->drawOffset*/)) {
				iys.Emplace(o, o->posY);
			}
		}
	});

	bulletTails.Foreach([&](auto& o)->bool { 
		if (o.UpdateCore()) return true;
		iys.Emplace(&o, o.posY);
		return false;
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
						.FillCirclePoints({}, o->radius, o->radians, 20, camera.scale * o->scale.x)
						.SetPosition(camera.ToGLPos(o->pos))
						.Draw();
					});
			}
		}
		if (human) {
			LineStrip()
				.FillCirclePoints({}, human->radius, human->radians, 20, camera.scale* human->scale.x)
				.SetPosition(camera.ToGLPos(human->pos))
				.Draw();
		}
	}

	// draw other effects?
	enm.Draw(camera);

	// todo: draw map border for debug ?

	// ui
	gLooper.DrawNode(rootNode);
};

void ScenePlay2::MakeSlime1(float radius, float safeRadius) {
	if (!human) return;
	XY pos;

	// circle gen , outside the human's safe range
	//static constexpr float radius = 400, safeRadius = 300, len = radius - safeRadius;
	float len = radius - safeRadius;
	auto r = std::sqrt(gLooper.rnd.Next<float>() * (len / radius) + safeRadius / radius ) * radius;
	auto a = gLooper.rnd.Next<float>(gNPI, gPI);
	pos.x = std::cos(a) * r;
	pos.y = std::sin(a) * r;
	pos += human->GetPhysPos();

	bmm.Add([this, pos] {
		this->im.Create<Slime>(pos);
	}, pos, Slime::cBornMaskAnchor, Slime::cBornMaskScale);
}

void ScenePlay2::MakeSlime2() {
	if (!human) return;
	XY pos;

	// full map random gen
	static constexpr XY ws{ gMapCfg.mapSize.x - 64, gMapCfg.mapSize.y - 64 };
	pos.x = gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.x - gMapCfg.physCellSize);
	pos.y = gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.y - gMapCfg.physCellSize);

	bmm.Add([this, pos] {
		this->im.Create<Slime>(pos);
	}, pos, Slime::cBornMaskAnchor, Slime::cBornMaskScale);
}

void ScenePlay2::MakeSlime3(XY const& pos) {
	bmm.Add([this, pos] {
		this->im.Create<Slime>(pos);
	}, pos, Slime::cBornMaskAnchor, Slime::cBornMaskScale);
}

#pragma endregion
