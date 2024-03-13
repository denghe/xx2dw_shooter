#include "pch.h"
#include "cfg.h"
#include "scene_test2.h"
#include "scene_main_menu.h"

#pragma region Monster2

void Monster2::Init(double hp_, int32_t mapPathIndex_) {
	hpBak = hp = hp_;
	hp *= (double)gSceneTest2->rnd.Next<float>(0.01f, 0.99f);
	radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
	mapPathIndex = mapPathIndex_;
	auto& tm = gSceneTest2->mapPaths[mapPathIndex_].tm;
	assert(radius <= tm.totalWidth);
	assert(radius >= tm.trackMargin);
	auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
	auto range = (tm.trackCount - numTrackCovered);
	trackIndex = numTrackCovered / 2 + gSceneTest2->rnd.Next<int32_t>(range);
	pointIndex = {};
	speed = cSpeed * cRadius / radius;
	radians = tm.GetRadians((int)pointIndex);
	pos = tm.GetPoint(trackIndex, (int)pointIndex);
}

bool Monster2::Update() {
	auto& tm = gSceneTest2->mapPaths[mapPathIndex].tm;
	pointIndex += speed;
	if (auto c = tm.GetPointCount(); pointIndex >= c) {
		// todo
		return true;
	}
	gSceneTest2->grid.Update(*this, tm.GetPoint(trackIndex, (int)pointIndex));
	radians = tm.GetRadians((int)pointIndex);
	return false;
}

void Monster2::Draw() {
	auto& camera = gSceneTest2->camera;
	auto& q = Quad::DrawOnce(gLooper.frame_circle);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * (radius / cRadius);
	q.radians = radians;
	q.colorplus = 1;
	q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
}

#pragma endregion

#pragma region SceneTest2

void SceneTest2::Init() {
	gSceneTest2 = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// ...

	// fill map context. layers name: platforms blocks path1...

	map = gLooper.map2;
	auto w = map->width;
	auto h = map->height;
	auto e = int(w * h);
	mapFrames.Resize(w * h);

	grid.Init(map->height, map->width, (int32_t)gCfg.unitSize);

	// fill default frame
	for (int i = 0, e = int(w * h); i < e; ++i) {
		mapFrames[i] = gLooper.frame_td_path;
	}

	// override blocks frame
	auto layer = map->FindLayer<TMX::Layer_Tile>("blocks");
	for (int i = 0; i < e; ++i) {
		auto gid = layer->gids[i];
		auto& gi = map->gidInfos[gid];
		auto& f = map->gidInfos[gid].frame;
		if (f) {
			mapFrames[i] = f;
		}
	}

	// override platforms frame
	layer = map->FindLayer<TMX::Layer_Tile>("platforms");
	for (int i = 0; i < e; ++i) {
		auto gid = layer->gids[i];
		auto& f = map->gidInfos[gid].frame;
		if (f) {
			mapFrames[i] = f;
		}
	}

	// search all layer prefix == "path" create MapPath
	for (auto& ly : map->flatLayers) {
		if (ly->name.starts_with("path")) {
			assert(ly->type == TMX::LayerTypes::TileLayer);
			layer = (TMX::Layer_Tile*)ly;
			mapPaths.Emplace().Init(map, layer, gCfg.unitSize);
		}
	}

	camera.SetScale(2.f);
	camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	camera.SetMaxFrameSize({ (float)gCfg.unitSize, (float)gCfg.unitSize });

	tasks.Add([this]()->xx::Task<> {
		//co_await gLooper.AsyncSleep(2);
		while (true)
		{
			for (size_t i = 0; i < 60; i++)
			{
				//if (grid.Count() >= gCfg.unitLimit) break;
				grid.MakeInit(rnd.Next<double>(gCfg.hpRange2.from, gCfg.hpRange2.to), 0);
			}
			co_yield 0;
		}
	});
}

void SceneTest2::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	grid.BufForeach([](Monster2& o)->GridForeachResult {
		return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
	});
}

void SceneTest2::Draw() {
	camera.Calc();

	for (int i = 0, ie = map->height; i < ie; ++i) {
		for (int j = 0, je = map->width; j < je; ++j) {
			Quad().SetFrame(mapFrames[i * je + j]).SetAnchor({ 0, 1 })
				.SetScale(camera.scale)
				.SetPosition(camera.ToGLPos(XY{ 32.f * j, 32.f * i }))
				.Draw();
		}
	}

	grid.BufForeach([camera = &camera](Monster2& o)->void {
		if (camera->InArea(o.pos)) {
			o.Draw();
		}
	});

	auto str = xx::ToString("total monster count = ", grid.Count());// , "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

#pragma endregion
