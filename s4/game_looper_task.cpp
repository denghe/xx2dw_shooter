#include "pch.h"

void GameLooper::Init() {
	printf("asdf\n");
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc24.Init();	// font init
    ctc72.Init();	//
	camera.SetMaxFrameSize({ gMaxFramePixelWidth, gMaxFramePixelHeight });		// camera init
	camera.SetScale(0.5);


    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");			// load texture packer data
	xx_assert(tp);
	tp->GetToByPrefix(frames_monster_1, "ma");
	tp->GetToByPrefix(frames_monster_2, "mb");
	tp->GetToByPrefix(frames_monster_3, "mc");
	tp->GetToByPrefix(frames_explosion, "e");
	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");
	xx_assert(frame_shooter);

	tiledMap = co_await AsyncLoadTiledMapFromUrl("res/m2.bmx", "res/");			// load tiled map data
	xx_assert(tiledMap);
	layerBG = tiledMap->FindLayer<TMX::Layer_Tile>("bg");
	xx_assert(layerBG);
	layerTrees = tiledMap->FindLayer<TMX::Layer_Tile>("trees");
	xx_assert(layerTrees);
	for (auto& img : tiledMap->images) img->texture->SetGLTexParm<GL_LINEAR>();	// set bg texture draw parm
	tileWidth = tiledMap->tileWidth;
	tileHeight = tiledMap->tileHeight;
	tileWidth_2 = tileWidth / 2;
	tileHeight_2 = tileHeight / 2;

	sgabTrees.Init(tiledMap->height, tiledMap->width, tileWidth, tileHeight);	// init physics grid index
	sgcMonsters.Init(tiledMap->height + gGridNumRows, tiledMap->width + gGridNumCols, gGridCellDiameter);


	for (int y = 0, ye = tiledMap->height; y < ye; ++y) {						// search & fill tree frame & trees
		for (int x = 0, xe = tiledMap->width; x < xe; ++x) {
			if (auto&& info = tiledMap->GetGidInfo(layerTrees, y, x)) {
				frame_tree = info->GetFrame();
				trees.emplace_back().Emplace()->Init(x * tileWidth, y * tileHeight);
			}
		}
	}

	float mapPixelWidth = tileWidth * tiledMap->width;
	float mapPixelHeight = tileHeight * tiledMap->height;


	shooter.Emplace()->Init({ mapPixelWidth / 2, mapPixelHeight / 2 });	// make player plane

	ready = true;

	// make monsters ( round by shooter )
	while (true) {
		auto basePos = shooter->pos;
		for (size_t i = 0; i < 5; i++) {
			auto a = rnd.Next<float>(M_PI * 2);
			auto r = rnd.Next<float>(1200, 12200);
			CreateMonster<Monster1>(basePos + XY{ std::cos(a), std::sin(a) } * r);
		}
		co_yield 0;
	}
	// 
	//tasks.Add([&]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 1; i++) {
	//			auto x = rnd.Next<float>(0, mapPixelWidth);
	//			CreateMonster<Monster1>(XY{ x, 0 });
	//		}
	//		co_yield 0;
	//		co_yield 0;
	//	}
	//});
	//tasks.Add([&]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 1; i++) {
	//			auto y = rnd.Next<float>(0, mapPixelHeight);
	//			CreateMonster<Monster1>(XY{ 0, y });
	//		}
	//		co_yield 0;
	//		co_yield 0;
	//		co_yield 0;
	//	}
	//});
	//tasks.Add([&]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 1; i++) {
	//			auto y = rnd.Next<float>(0, mapPixelHeight);
	//			CreateMonster<Monster1>(XY{ mapPixelWidth, y });
	//		}
	//		co_yield 0;
	//		co_yield 0;
	//		co_yield 0;
	//	}
	//});

	while (true) co_yield 0;	// idle for hold memory
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;

	shooter->mainLogic();
	for (auto& tree : trees) { tree->mainLogic(); }
	bullets_shooter1.Foreach([&](auto& o) { return o->mainLogic.Resume(); });
	monsters.Foreach([&](auto& o) { return o->mainLogic.Resume(); });
	effects_explosion.Foreach([&](auto& o) { return o->mainLogic.Resume(); });
	effects_damageText.Foreach([&](auto& o) { return o->mainLogic.Resume(); });

	// todo: more Update
}

void GameLooper::Draw() {

	if (ready) {
		camera.SetOriginal(shooter->pos);
		camera.Calc();

		auto& tm = *tiledMap;
		for (auto& a : tm.anims) {
			a->Update(delta);
		}

		auto scaledTileSize = tm.GetScaledTileSize(camera);
		auto basePos = tm.GetBasePos(camera);
		Quad q;
		q.SetScale(camera.scale).SetAnchor({ 0, 0 });
		
		int minX = tm.GetMinColumnIndex(camera);
		int maxX = tm.GetMaxColumnIndex(camera, 1);
		int minY = tm.GetMinRowIndex(camera);
		int maxY = tm.GetMaxRowIndex(camera, 1);
		for (int y = minY; y < maxY; ++y) {
			for (int x = minX; x < maxX; ++x) {
				if (auto&& info = tm.GetGidInfo(layerBG, y, x)) {
					q.SetPosition(basePos + XY::Make(x, -y) * scaledTileSize).TrySetFrame(info->GetFrame()).Draw();
				}
			}
		}
		
		//int maxTreeY = tm.GetMaxRowIndex(camera, 2);
		//for (int y = minY; y < maxTreeY; ++y) {
		//	for (int x = minX; x < maxX; ++x) {
		//		if (auto&& info = tm.GetGidInfo(layerTrees, y, x)) {
		//			q.SetPosition(basePos + XY::Make(x, -y) * scaledTileSize).TrySetFrame(info->GetFrame()).Draw();
		//		}
		//	}
		//}
		//for (auto& tree : trees) {
		//	tree->Draw();
		//}
		//shooter->Draw();
		//bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// order by Y

		tmpsPosYObj.emplace_back(shooter->pos.y, shooter.pointer);

		for (auto& tree : trees) {
			if (camera.InArea(tree->pos)) {
				tmpsPosYObj.emplace_back(tree->pos.y, tree.pointer);
			}
		}

		bullets_shooter1.Foreach([&](auto& o) {
			if (camera.InArea(o->pos)) {
				tmpsPosYObj.emplace_back(o->pos.y, o.pointer);
			}
		});

		monsters.Foreach([&](auto& o) {
			if (camera.InArea(o->pos)) {
				tmpsPosYObj.emplace_back(o->pos.y, o.pointer);
			}
		});

		effects_explosion.Foreach([&](auto& o) {
			if (camera.InArea(o->pos)) {
				tmpsPosYObj.emplace_back(o->pos.y, o.pointer);
			}
		});

		std::sort(tmpsPosYObj.begin(), tmpsPosYObj.end(), [](auto const& a, auto const& b) {
			return a.first < b.first;
		});

		for (auto& [_, obj] : tmpsPosYObj) {
			obj->Draw();
		}
		tmpsPosYObj.clear();


		effects_damageText.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw
		ctc72.Draw({ -gEngine->windowWidth_2, gEngine->windowHeight_2 - ctc72.canvasHeight_2 }, "A S D W move, Z X zoom, MOUSE fire");
	}
	fv.Draw(ctc72);       // draw fps at corner
}


MonsterBase* GameLooper::FindNeighborMonster(XY const& pos, float radius) {
	auto p = gGridBasePos.MakeAdd(pos);
	auto crIdx = sgcMonsters.PosToCrIdx(p);
	MonsterBase* r{};
	sgcMonsters.Foreach9(crIdx, [&](MonsterBase* m)->bool {
		// (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
		auto d = m->pos - pos;
		auto rr = (m->radius + radius) * (m->radius + radius);
		auto dd = d.x * d.x + d.y * d.y;
		if (rr > dd) {
			r = m;
			return true;
		}
		return false;
		});
	return r;
}

MonsterBase* GameLooper::FindNearestMonster(XY const& pos, float maxDistance) {
	auto p = gGridBasePos.MakeAdd(pos);						// convert pos to grid coordinate
	auto crIdx = sgcMonsters.PosToCrIdx(p);					// calc grid col row index

	float minVxxyy = maxDistance * maxDistance;
	MonsterBase* o{};
	XY ov;

	auto& lens = sgrdd.lens;
	auto& idxs = sgrdd.idxs;
	for (int i = 1; i < lens.len; i++) {
		if (lens[i].radius > maxDistance) break;			// limit search range

		auto offsets = &idxs[lens[i - 1].count];
		auto size = lens[i].count - lens[i - 1].count;
		sgcMonsters.ForeachCells(crIdx, offsets, size, [&](MonsterBase* m)->bool {
			auto v = m->pos - pos;
			if (auto xxyy = v.x * v.x + v.y * v.y; xxyy < minVxxyy) {
				minVxxyy = xxyy;
				o = m;
				ov = v;
			}
			return false;
			});

		if (o) return o;									// found. stop ring diffuse step
	}
	return nullptr;
}
