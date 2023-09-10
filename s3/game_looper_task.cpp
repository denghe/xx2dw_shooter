#include "pch.h"

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();	// font init

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

	shooter.Emplace()->Init({});	// make player plane

	ready = true;
	co_return;
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.2)) {
		scale -= 0.25;
		if (scale < 0.25) {
			scale = 0.25;
		}
		zoom = 1 / scale;
		//w2 = w / 2 * zoom;
		//h2 = h / 2 * zoom;
	} else if (KeyDownDelay(KeyboardKeys::X, 0.2)) {
		scale += 0.25;
		zoom = 1 / scale;
		//w2 = w / 2 * zoom;
		//h2 = h / 2 * zoom;
	}
	if (!ready) return;

	shooter();
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (ready) {
		auto& tm = *tiledMap;
		for (auto& a : tm.anims) {
			a->Update(delta);
		}

		int mapTileWidth = tm.tileWidth, mapTileHeight = tm.tileHeight;
		float mapScaledTileWidth = mapTileWidth * scale, mapScaledTileHeight = mapTileHeight * scale;
		int mapNumColumns = tm.width, mapNumRows = tm.height;

		auto& sp = shooter->pos;
		auto basePos = XY{ -sp.x, float(-mapTileHeight) + sp.y } * scale;
		Quad q;
		q.SetScale(scale).SetAnchor({0, 0});

		auto w2 = windowWidth / 2 * zoom, h2 = windowHeight / 2 * zoom;
		auto minX = (sp.x - w2) / mapTileWidth;
		auto maxX = (sp.x + w2) / mapTileWidth;
		auto minY = (sp.y - h2) / mapTileHeight;
		auto maxY = (sp.y + h2) / mapTileHeight;
		auto maxTreeY = maxY + 1;	// tree tile height == mapTileHeight * 2
		if (minX < 0) minX = 0; else if (minX > mapNumColumns) minX = mapNumColumns;
		if (maxX < 0) maxX = 0; else if (maxX > mapNumColumns) maxX = mapNumColumns;
		if (minY < 0) minY = 0; else if (minY > mapNumRows) minY = mapNumRows;
		if (maxY < 0) maxY = 0; else if (maxY > mapNumRows) maxY = mapNumRows;
		if (maxTreeY < 0) maxTreeY = 0; else if (maxTreeY > mapNumRows) maxTreeY = mapNumRows;

		for (int y = minY; y < maxY; ++y) {
			for (int x = minX; x < maxX; ++x) {
				if (auto&& info = tm.GetGidInfo(layerBG, y, x)) {
					q.SetPosition(basePos + XY{ x * mapScaledTileWidth, -y * mapScaledTileHeight }).TrySetFrame(info->GetFrame()).Draw();
				}
			}
		}

		for (int y = minY; y < maxTreeY; ++y) {
			for (int x = minX; x < maxX; ++x) {
				if (auto&& info = tm.GetGidInfo(layerTrees, y, x)) {
					q.SetPosition(basePos + XY{ x * mapScaledTileWidth, -y * mapScaledTileHeight }).TrySetFrame(info->GetFrame()).Draw();
				}
			}
		}

		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
