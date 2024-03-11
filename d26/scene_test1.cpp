#include "pch.h"
#include "looper.h"
#include "cfg.h"
#include "scene_test1.h"

#pragma region SnakeBody

void SnakeBody::Init(XY const& pos_, GridWeak head_, GridWeak prev_, bool isTail_) {
	head = head_;
	prev = prev_;
	isTail = isTail_;
	pos = pos_;
}

XY SnakeBody::GenRndPos(float radius, float safeRadius) {
	auto& rnd = gSceneTest1->rnd;
	float len = radius - safeRadius;
	auto r = std::sqrt(rnd.Next<float>() * (len / radius) + safeRadius / radius) * radius;
	auto a = rnd.Next<float>(gNPI, gPI);
	auto p = pos + XY{ std::cos(a) * r, std::sin(a) * r };
	// map edge limit
	if (p.x < gCfg.mapSafeMinPos.x) {
		p.x = gCfg.mapSafeMinPos.x - (p.x - gCfg.mapSafeMinPos.x);
	} else if (p.x >= gCfg.mapSafeMaxPos.x) {
		p.x = gCfg.mapSafeMaxPos.x - (p.x - gCfg.mapSafeMaxPos.x);
	}
	if (p.y < gCfg.mapSafeMinPos.y) {
		p.y = gCfg.mapSafeMinPos.y - (p.y - gCfg.mapSafeMinPos.y);
	} else if (p.y >= gCfg.mapSafeMaxPos.y) {
		p.y = gCfg.mapSafeMaxPos.y - (p.y - gCfg.mapSafeMaxPos.y);
	}
	return p;
}

int SnakeBody::UpdateCore() {
	auto& grid = gSceneTest1->grid;
	COR_BEGIN
	// is body: follow prev
	while (grid.Exists(prev)) {
		{
			auto& p = grid.Get(prev);
			auto hPos = p.pos;
			auto v = hPos - pos;
			auto distance = std::sqrt(v.x * v.x + v.y * v.y);
			if (auto d = distance - cDistance; d > 0) {
				auto inc = v / distance * std::min(d, cSpeed);
				grid.Update(*this, pos + inc);
			}
		}
		COR_YIELD
	}
	// is head: select random pos & gogogo
	while (true) {
		hPos = GenRndPos(300, 50);
		while (true) {
			{
				auto v = hPos - pos;
				auto r = std::atan2(v.y, v.x);
				RotateControl::Step(radians, r, cMinRadians);
				XY inc{ std::cos(radians), std::sin(radians) };
				if (v.x * v.x + v.y * v.y > cSpeed * cSpeed) {
					grid.Update(*this, pos + inc * cSpeed);
				} else {
					grid.Update(*this, hPos);
					break;
				}
			}
			COR_YIELD
		}
	}
	COR_END
}

bool SnakeBody::Update() {
	return (lineNumber = UpdateCore()) == 0;
}

void SnakeBody::Draw() {
	auto& grid = gSceneTest1->grid;
	int idx;
	if (grid.Exists(prev)) {
		if (isTail) idx = 2;
		else idx = 1;
	} else {
		idx = 0;
	}
	auto& q = Quad::DrawOnce(gLooper.frames_snake[idx]);
	q.pos = gSceneTest1->camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(gSceneTest1->camera.scale) * cScale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::CreateSnake(XY const& headPos, int len) {
	auto& h = grid.MakeInit(headPos, GridWeak{}, GridWeak{}, false);
	h.radians = -gPI / 2;
	auto hgw = h.ToGridWeak();
	auto gw = hgw;
	for (int i = 0; i < len; i++) {
		auto& o = grid.Get(gw);
		gw = grid.MakeInit(o.pos + XY{ 0, o.cDistance }, hgw, gw).ToGridWeak();
	}
	auto& o = grid.Get(gw);
	grid.MakeInit(o.pos + XY{ 0, o.cDistance }, hgw, gw, true);
}

void SceneTest1::Init() {
	gSceneTest1 = this;

	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"clear", [&]() {
		grid.Clear();
	});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 100 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1", [&]() {
			CreateSnake(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m, gDesign.xy4a, gLooper.s9cfg_btn, U"+100", [&]() {
		for (int i = 0; i < 100; i++) {
			CreateSnake(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		}
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 100 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1000", [&]() {
		for (int i = 0; i < 1000; i++) {
			CreateSnake(gCfg.mapCenterPos, rnd.Next<int>(10, 30));
		}
		});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});


	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gCfg.mapCenterPos);
	camera.SetScale(1.f);

	grid.Init(gCfg.physNumRows, gCfg.physNumCols, gCfg.physCellSize);
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	// hit control
	auto& m = gLooper.mouse;
	if (m.btnStates[0] && !gLooper.mouseEventHandler) {
		auto p = camera.ToLogicPos(m.pos);
		grid.ForeachByRange(gLooper.sgrdd, p, gCfg.mouseHitRange, [&](SnakeBody& o) {
			if (grid.Exists(o.prev)) {
				grid.Get(o.prev).isTail = true;
			}
			grid.Remove(o);
		});
	}

	grid.BufForeach([](SnakeBody& o)->void {
		o.Update();
	});
}

void SceneTest1::Draw() {
	camera.Calc();

#if 1
	grid.BufForeach([](SnakeBody& o)->void {
		o.Draw();
	});
#else
	// fill phys items to iys order by row & cut by cell pos
	int32_t rowFrom, rowTo, colFrom, colTo;
	camera.FillRowColIdxRange(gCfg.physNumRows, gCfg.physNumCols, gCfg.physCellSize,
		rowFrom, rowTo, colFrom, colTo);
	auto& iys = im.iys;
	for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
		for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
			auto idx = sgcPhysItems.CrIdxToCellIdx({ colIdx, rowIdx });
			sgcPhysItems.ForeachWithoutBreak(idx, [&](PhysSceneItem* o) {
				iys.Emplace(o, o->posY);
				});
		}
	}
	im.Draw(camera);
#endif

	LineStrip().FillCirclePoints({}, gCfg.mouseHitRange, {}, 100, XY::Make(camera.scale))
		.SetPosition(gLooper.mouse.pos)
		.Draw();

	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: Z / X   hit: mouse click", RGBA8_Green, { 0.5f, 1 });

	auto str = xx::ToString("total item count = ", grid.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

#pragma endregion
