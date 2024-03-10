#include "pch.h"
#include "game_looper.h"
#include "game_cfg.h"
#include "scene_main_menu.h"
#include "scene_test1.h"

#pragma region SceneTest1

void Monster1::Init(double hp_) {
	hpBak = hp = hp_;
	hp *= (double)gScene->rnd.Next<float>(0.01f, 0.99f);
	radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
	auto& tm = gScene->tm;
	assert(radius <= tm.totalWidth);
	assert(radius >= tm.trackMargin);
	auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
	auto range = (tm.trackCount - numTrackCovered);
	trackIndex = numTrackCovered / 2 + gScene->rnd.Next<int32_t>(range);
	pointIndex = {};
	speed = cSpeed * cRadius / radius;
	radians = tm.GetRadians((int)pointIndex);
	pos = tm.GetPoint(trackIndex, (int)pointIndex);
}

bool Monster1::Update() {
	auto& tm = gScene->tm;
	pointIndex += speed;
	if (auto c = tm.GetPointCount(); pointIndex >= c) {
		//pointIndex -= c;	// todo
		return true;
	}
	gScene->grid.Update(*this, tm.GetPoint(trackIndex, (int)pointIndex));
	radians = tm.GetRadians((int)pointIndex);
	return false;
}

void Monster1::Draw() {
	auto& camera = gScene->camera;
	auto& q = Quad::DrawOnce(gLooper.frame_circle);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * (radius / cRadius);
	q.radians = radians;
	q.colorplus = 1;
	q.color = { cColor.r, cColor.g, cColor.b, uint8_t(25 + 230 * (hp / hpBak)) };
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	gScene = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	camera.SetScale(1.f);
	camera.SetOriginal(gCfg.mapSize_2);
	camera.SetMaxFrameSize({ gCfg.unitSize, gCfg.unitSize });

	grid.Init(gCfg.gridNumRows, gCfg.gridNumCols, gCfg.gridCellSize);

	std::vector<CurvePoint> cps;
	cps.emplace_back(gCfg.mapSize_2 + XY{ -400, -200 });
	cps.emplace_back(gCfg.mapSize_2 + XY{ 400, -200 });
	cps.emplace_back(gCfg.mapSize_2 + XY{ 400, 200 });
	cps.emplace_back(gCfg.mapSize_2 + XY{ -400, 200 });
	tm.Init(cps);

	tasks.Add([this]()->xx::Task<> {
		//co_await gLooper.AsyncSleep(2);
		while (true) {
			for (size_t i = 0; i < 80; i++) {
				if (grid.Count() >= gCfg.unitLimit) break;
				grid.MakeInit(rnd.Next<double>(gCfg.hpRange.from, gCfg.hpRange.to));
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

	grid.BufForeach([](Monster1& o)->GridForeachResult {
		return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
	});
}

void SceneTest1::Draw() {
	camera.Calc();

#if 1
	// draw camera range cells( slow 1/5 )
	int32_t rowFrom, rowTo, colFrom, colTo;
	camera.FillRowColIdxRange(grid.numRows, grid.numCols, grid.cellSize, rowFrom, rowTo, colFrom, colTo);
	for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
		for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
			auto cidx = grid.numCols * rowIdx + colIdx;
			grid.CellForeach(cidx, [&](Monster1& o)->void {
				o.Draw();
			});
		}
	}
#else
	// draw all
	grid.BufForeach([camera = &camera](Monster1& o)->void {
		if (camera->InArea(o.pos)) {
			o.Draw();
		}
	});
#endif

	auto str = xx::ToString("total monster count = ", grid.Count());// , "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

#pragma endregion


//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"Save", [&]() {
//	Save();
//	});
//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"Load", [&]() {
//	Load();
//	});
//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1", [&]() {
//		numBulletGenerateByEveryFrame = 1;
//	});
//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+10", [&]() {
//		numBulletGenerateByEveryFrame = 10;
//	});
//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+100", [&]() {
//		numBulletGenerateByEveryFrame = 100;
//	});
//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1000", [&]() {
//		numBulletGenerateByEveryFrame = 1000;
//	});
