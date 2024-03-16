#include "pch.h"
//#include "looper.h"
//#include "cfg.h"
//#include "scene_main_menu.h"
//#include "scene_test1.h"
//
//#pragma region SceneTest1
//
//void Monster1::Init(double hp_) {
//	hpBak = hp = hp_;
//	hp *= (double)gSceneTest1->rnd.Next<float>(0.01f, 0.99f);
//	radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
//	auto& tm = gSceneTest1->tm;
//	assert(radius <= tm.totalWidth);
//	assert(radius >= tm.trackMargin);
//	auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
//	auto range = (tm.trackCount - numTrackCovered);
//	trackIndex = numTrackCovered / 2 + gSceneTest1->rnd.Next<int32_t>(range);
//	pointIndex = {};
//	speed = cSpeed * cRadius / radius;
//	radians = tm.GetRadians((int)pointIndex);
//	pos = tm.GetPoint(trackIndex, (int)pointIndex);
//}
//
//bool Monster1::Update() {
//	auto& tm = gSceneTest1->tm;
//	pointIndex += speed;
//	if (auto c = tm.GetPointCount(); pointIndex >= c) {
//		//pointIndex -= c;	// todo
//		return true;
//	}
//	gSceneTest1->grid.Update(*this, tm.GetPoint(trackIndex, (int)pointIndex));
//	radians = tm.GetRadians((int)pointIndex);
//	return false;
//}
//
//void Monster1::Draw() {
//	auto& camera = gSceneTest1->camera;
//	auto& q = Quad::DrawOnce(gLooper.frame_td_shape_circle);
//	q.pos = camera.ToGLPos(pos);
//	q.anchor = cAnchor;
//	q.scale = XY::Make(camera.scale) * (radius / cRadius);
//	q.radians = radians;
//	q.colorplus = 1;
//	q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
//}
//
//#pragma endregion
//
//#pragma region SceneTest1
//
//void SceneTest1::Init() {
//	gSceneTest1 = this;
//
//	rootNode.Emplace()->Init();
//	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
//		gLooper.DelaySwitchTo<SceneMainMenu>();
//	});
//
//	camera.SetScale(1.f);
//	camera.SetOriginal(gCfg.cameraOriginal);
//	camera.SetMaxFrameSize({ gCfg.unitSize, gCfg.unitSize });
//
//	grid.Init(gLooper.map1->height + 5, gLooper.map1->width + 5, gCfg.gridCellSize);
//
//	std::vector<CurvePoint> cps;
//	static constexpr XY fix{ 0, -45};
//	cps.emplace_back(XY{ 5, 415 } + fix);
//	cps.emplace_back(XY{ 315, 400 } + fix);
//	cps.emplace_back(XY{ 524, 190 } + fix);
//	cps.emplace_back(XY{ 684, 190 } + fix);
//	cps.emplace_back(XY{ 845, 190 } + fix);
//	cps.emplace_back(XY{ 990, 400 } + fix);
//	cps.emplace_back(XY{ 687, 710 } + fix);
//	cps.emplace_back(XY{ 687, 810 } + fix);
//	tm.Init(cps);
//
//	// set target layer for easy use
//	layer = gLooper.map1->FindLayer<TMX::Layer_Tile>("Tile Layer 1");
//	assert(layer && layer->type == TMX::LayerTypes::TileLayer);
//
//	tasks.Add([this]()->xx::Task<> {
//		//co_await gLooper.AsyncSleep(2);
//		while (true) {
//			for (size_t i = 0; i < 1; i++) {
//				if (grid.Count() >= gCfg.unitLimit) break;
//				grid.MakeInit(rnd.Next<double>(gCfg.hpRange.from, gCfg.hpRange.to));
//			}
//			co_yield 0;
//		}
//	});
//}
//
//void SceneTest1::Update() {
//	// scale control
//	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
//		camera.IncreaseScale(0.1f, 5);
//	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
//		camera.DecreaseScale(0.1f, 0.1f);
//	}
//	camera.Calc();
//
//	grid.BufForeach([](Monster1& o)->GridForeachResult {
//		return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
//	});
//}
//
//void SceneTest1::Draw() {
//	camera.Calc();
//
//	// draw tiled map
//	for (int i = 0, ie = gLooper.map1->height; i < ie; ++i) {
//		for (int j = 0, je = gLooper.map1->width; j < je; ++j) {
//			auto idx = i * je + j;
//			if (auto gid = layer->gids[idx]) {
//				auto& gi = gLooper.map1->gidInfos[gid];
//				Quad().SetFrame(gi.frame).SetAnchor({ 0, 1 })
//					.SetScale(camera.scale)
//					.SetPosition(camera.ToGLPos(XY{ 32.f * j, 32.f * i }))
//					.Draw();
//			}
//		}
//	}
//
//
//#if 0
//	// draw camera range cells( slow 1/5 )
//	int32_t rowFrom, rowTo, colFrom, colTo;
//	camera.FillRowColIdxRange(grid.numRows, grid.numCols, grid.cellSize, rowFrom, rowTo, colFrom, colTo);
//	for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
//		for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
//			auto cidx = grid.numCols * rowIdx + colIdx;
//			grid.CellForeach(cidx, [&](Monster1& o)->void {
//				o.Draw();
//			});
//		}
//	}
//#else
//	// draw all
//	grid.BufForeach([camera = &camera](Monster1& o)->void {
//		if (camera->InArea(o.pos)) {
//			o.Draw();
//		}
//	});
//#endif
//
//	auto str = xx::ToString("total monster count = ", grid.Count());// , "  total blood text count = ", enm.ens.Count());
//	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });
//
//	gLooper.DrawNode(rootNode);
//}
//
//#pragma endregion
