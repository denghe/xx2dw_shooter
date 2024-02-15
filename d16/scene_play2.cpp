#include <pch.h>
#include <all.h>

#pragma region Scene[Phys]Item

void SceneItem::SceneItemInit(int typeId_, ItemManagerBase* im_) {
	ItemInit(typeId_, im_);
	scene = (ScenePlay2*)im_->userData;
}

XY ScenePhysItem::GetPhysPos() {
	return pos + physOffset;
}

void ScenePhysItem::PhysAdd() {
	SGCAdd(scene->sgcPhysItems, (pos + physOffset).As<int>());
}

void ScenePhysItem::PhysUpdate() {
	SGCUpdate((pos + physOffset).As<int>());
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
		scene->camera.SetOriginal(pos);

		co_yield 0;
	}
}

void Human::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_human_1[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos);
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
	pos = pos_;
	radius = cRadius;
	physOffset = { 0, -radius / 2 };
	PhysAdd();
}

int Slime::UpdateCore() {
	return !mainTask.Resume();
}

xx::Task<> Slime::MainTask() {
	for (int e = gLooper.frameNumber + cLifeNumFrames; gLooper.frameNumber < e;) {
		// todo: move ?
		// PhysUpdate();
		co_yield 0;
	}
	// todo: fadeout ?
}

void Slime::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frames_creature_slime[(int)frameIndex]);
	q.pos = camera.ToGLPos(pos);
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
		this->physBorderVisible = !this->physBorderVisible;
	});

	// init camera
	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
	//camera.SetScale(4);

	// init objs
	sgcPhysItems.Init(gMapCfg.physNumRows, gMapCfg.physNumCols, gMapCfg.physCellSize);
	im.Init(this);
	im.Create<Human>();
}

//#define ENABLE_SORT1

void ScenePlay2::Update() {
	// zoom control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.DecreaseScale(0.02f, 0.02f);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.IncreaseScale(0.02f, 5);
	}

	// simulate room master logic
	for (int i = 0; i < 30; i++) {
		MakeSlime();
	}

	// update objs
	bmm.Update();
	im.Update();
#ifdef ENABLE_SORT1
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
#ifdef ENABLE_SORT1
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
	if (physBorderVisible) {
		for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
			for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
				auto idx = sgcPhysItems.CrIdxToCellIdx({ colIdx, rowIdx });
				sgcPhysItems.ForeachWithoutBreak(idx, [&](ScenePhysItem* o) {
					LineStrip()
						.FillCirclePoints({}, o->radius, o->radians, 20, XY::Make(camera.scale * o->scale.x))
						.SetPosition(camera.ToGLPos(o->GetPhysPos()))
						.Draw();
					});
			}
		}
	}

	// todo: draw map border for debug ?

	// ui
	gLooper.DrawNode(rootNode);
};

void ScenePlay2::MakeSlime() {
	constexpr XY ws{ gMapCfg.mapSize.x - 64, gMapCfg.mapSize.y - 64 };
	XY pos{
		gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.x - gMapCfg.physCellSize)
		, gLooper.rnd.Next<float>(gMapCfg.physCellSize, ws.y - gMapCfg.physCellSize)
	};
	bmm.Add([this, pos] {
		this->im.Create<Slime>(pos);
	}, pos, Slime::cBornMashAnchor, Slime::cBornMashScale);
}

#pragma endregion
