#include "pch.h"

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();	// font init
	camera.SetMaxFrameSize({ gMaxFramePixelWidth, gMaxFramePixelHeight });	// camera init

    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");		// load texture packer data
	xx_assert(tp);
	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");
	xx_assert(frame_shooter);

	tiledMap = co_await AsyncLoadTiledMapFromUrl("res/m.bmx");	// load tiled map data
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

	for (int y = 0, ye = tiledMap->height; y < ye; ++y) {						// search & fill tree frame & trees
		for (int x = 0, xe = tiledMap->width; x < xe; ++x) {
			if (auto&& info = tiledMap->GetGidInfo(layerTrees, y, x)) {
				frame_tree = info->GetFrame();
				trees.emplace_back().Emplace()->Init(x * tileWidth, y * tileHeight);
			}
		}
	}

	shooter.Emplace()->Init({});	// make player plane

	ready = true;
	co_return;
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.2)) {
		camera.DecreaseScale(0.25, 0.25);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.2)) {
		camera.IncreaseScale(0.25, 10);
	}
	if (!ready) return;

	shooter();
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });
	for (auto& tree : trees) { tree(); }

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

		// order by Y draw shooter + tree + bullets

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

		std::sort(tmpsPosYObj.begin(), tmpsPosYObj.end(), [](auto const& a, auto const& b) {
			return a.first < b.first;
		});

		for (auto& [_, obj] : tmpsPosYObj) {
			obj->Draw();
		}
		tmpsPosYObj.clear();


		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
