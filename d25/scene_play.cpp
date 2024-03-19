#include "pch.h"
#include "scene_play.h"
#include "scene_main_menu.h"

xx::Task<> ScenePlay::MainTask() {
	while (true)
	{
		//for (size_t i = 0; i < 20; i++)
		{
			//if (grid.Count() >= gCfg.unitLimit) break;
			// rnd.Next<double>(gCfg.hpRange2.from, gCfg.hpRange2.to)
			grids.MakeInit<::Enemy::Monster2>(60, rnd.Next<int32_t>(0, tms.len - 1));
		}
		
		// sleep 1 secs
		for (int32_t i = 0; i < (int32_t)gDesign.fps; ++i) {
			co_yield 0;
		}
	}
}

void ScenePlay::Init() {
	gScenePlay = this;
	mainTask = MainTask();

	ringShader.Init(&gLooper);

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ -300, 0}, gDesign.xy8a, gLooper.s9cfg, U"pause", [&]() {
		gameSpeedRate = 0;
	});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ -150, 0}, gDesign.xy8a, gLooper.s9cfg, U"speed: 1", [&]() {
		gameSpeedRate = 1;
	});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m, gDesign.xy8a, gLooper.s9cfg, U"speed: 5", [&]() {
		gameSpeedRate = 5;
	});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ 150, 0 }, gDesign.xy8a, gLooper.s9cfg, U"speed: 50", [&]() {
		gameSpeedRate = 50;
	});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ 300, 0 }, gDesign.xy8a, gLooper.s9cfg, U"speed: 1000", [&]() {
		gameSpeedRate = 1000;
	});

	// fill map context

	map = gLooper.map_stages[0];
	layer = map->FindLayer<TMX::Layer_Tile>("map");

	auto w = map->width;
	auto h = map->height;
	auto e = int(w * h);
	mapMaxX = gCfg.unitSize * w;
	mapMaxY = gCfg.unitSize * h;

	grids.InitAll(h, w, (int32_t)gCfg.unitSize);

	// todo: get init tower config from map layer: fg2
	//auto fgLayer = map->FindLayer<TMX::Layer_Tile>("fg2");


	// todo: get game config from db gen code ?


	for (int i = 0; i < e; ++i) {
		auto gid = layer->gids[i];
		if (auto& gi = map->gidInfos[gid]) {
			if (gi.frame == gLooper.frame_td_cell_space) {
				// make some Tower
				grids.MakeInit<::Tower::Arrow>(i - i / w * w, i / w);
			}
		}
	}

	// fill tms by layer: path
	auto pathLayer = map->FindLayer<TMX::Layer_Object>("path");
	tms.Reserve((int32_t)pathLayer->objects.size());
	std::vector<CurvePoint> ps;
	for (auto& o : pathLayer->objects) {
		assert(o->type == TMX::ObjectTypes::Polygon);
		auto& polygon = (xx::Ref<TMX::Object_Polygon>&)o;
		ps.reserve(polygon->points.size());
		for (auto& p : polygon->points) {
			ps.emplace_back(XY{ (float)(polygon->x + p.x), (float)(polygon->y + p.y) }, 0.05f, 50);
		}
		auto& tm = tms.Emplace();
		tm.totalWidth = gCfg.unitSize;
		tm.trackCount = 11;
		tm.pointDistance = 0.1f;
		tm.trackMargin = tm.totalWidth / tm.trackCount;
		tm.Init(ps);
		ps.clear();
	}

	camera.SetScale(2.f);
	camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	camera.SetMaxFrameSize({ (float)gCfg.unitSize, (float)gCfg.unitSize });

	em.Init(10000);
	enm.Init(10000);
}

void ScenePlay::BeforeUpdate() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// handle mouse drag map change camera original
	if (!gLooper.mouseEventHandler) {
		auto& m = gLooper.mouse;
		auto mbs = m.btnStates[0];

		auto mp = m.pos / camera.scale;
		mp.x = -mp.x;

		if (lastMBState != mbs) {
			lastMBState = mbs;
			if (mbs) {							// mouse down
				mouseOffset = mp - camera.original;
				lastMousePos = m.pos;
			} else {							// mouse up
				if (dragging) {					// dragging end
					dragging = false;
				} else {						// click
					focus.Reset();
					auto pos = camera.ToLogicPos(m.pos);
					if (auto a = grids.Get<Tower::Arrow>().TryGetCellItemByPos(pos)) {
						assert(a->__grid_next == -1);				// logic ensure 1 cell 1 item
						focus = a->ToGridsWeak();
					} else if (auto c = grids.Get<Tower::Cannon>().TryGetCellItemByPos(pos)) {
						assert(a->__grid_next == -1);				// logic ensure 1 cell 1 item
						focus = a->ToGridsWeak();
					}
					// ...
					// todo: play anim when first focus?
					// todo: clicked item bounce zoom 20% anim
				}
			}
		}
		if (mbs && lastMousePos != m.pos) {		// mouse down + moved == dragging
			dragging = true;
			camera.original = mp - mouseOffset;
		}
	}
	camera.Calc();
}

void ScenePlay::Update() {
	for (int32_t i = 0; i < gameSpeedRate; ++i) {
		++frameNumber;

		mainTask();		// gen monster

		if (gLooper.KeyDownDelay(KeyboardKeys::Escape, 0.02f)) {
			focus.Reset();
		}

		grids.ForeachAll([&]<typename T>(Grid<T>&grid) {
			grid.BufForeach([](T& o)->GridForeachResult {
				return o.Update() ? GridForeachResult::RemoveAndContinue : GridForeachResult::Continue;
				});
		});

		enm.Update();
		em.Update();
	}
}

void ScenePlay::Draw() {

	for (int i = 0, ie = map->height; i < ie; ++i) {
		for (int j = 0, je = map->width; j < je; ++j) {
			auto gid = layer->gids[i * je + j];
			if (auto& gi = map->gidInfos[gid]) {
				Quad().SetFrame(gi.frame).SetAnchor({ 0, 1 })
					.SetScale(camera.scale)
					.SetPosition(camera.ToGLPos(XY{ 32.f * j, 32.f * i }))
					.Draw();
			}

		}
	}

	grids.ForeachAll([&]<typename T>(Grid<T>&grid) {
		grid.BufForeach([camera = &camera](T& o)->void {
			if (camera->InArea(o.pos)) {
				o.Draw();
			}
		});
	});


	auto& m = gLooper.mouse;
	auto mp = camera.ToLogicPos(m.pos);
	auto mc = (int32_t)mp.x / (int32_t)gCfg.unitSize;
	auto mr = (int32_t)mp.y / (int32_t)gCfg.unitSize;
	Quad().SetFrame(gLooper.frame_td_cell_mouse_focus).SetAnchor({ 0, 1 })
		.SetScale(camera.scale)
		.SetPosition(camera.ToGLPos(XY{ mc * gCfg.unitSize, mr * gCfg.unitSize }))
		.Draw();


	enm.Draw(camera);
	em.Draw();

	if (auto o = (ItemBase*)grids.TryGetBase(focus)) {
		o->Focus();
	}

	auto str = xx::ToString("total item count = ", grids.Count());// , "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 40 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}
