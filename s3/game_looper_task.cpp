#include "pch.h"

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
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
	//cam.Init({w, h}, *tiledMap);
	//cam.SetPosition({ 830, 510 });
	//shooter.Emplace()->Init(cam.pos);	// make player plane
	scale = gScale / 4;
	zoom = 1 / scale;

	shooter.Emplace()->Init({});	// make player plane

	ready = true;
	co_return;
}

void GameLooper::Update() {
	fv.Update();
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

		float tiw = tm.tileWidth * scale, tih = tm.tileHeight * scale;
		float tmw = tm.width, tmh = tm.height;
		float totalWidth = tiw * tmw, totalHeight = tih * tmh;

		Quad q;
		q.SetScale(scale).SetAnchor({0, 0});
		auto basePos = XY{ 0, -tih } - shooter->pos;

		for (int y = 0; y < (int)tmh; ++y) {
			for (int x = 0; x < (int)tmw; ++x) {
				if (auto&& info = tm.GetGidInfo(layerBG, y, x)) {
					q.SetPosition(basePos + XY{ x * tiw, -y * tih }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}
		for (int y = 0; y < (int)tmh; ++y) {
			for (int x = 0; x < (int)tmw; ++x) {
				if (auto&& info = tm.GetGidInfo(layerTrees, y, x)) {
					q.SetPosition(basePos + XY{ x * tiw, -y * tih }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}

		//cam.SetPosition(cam.worldPixel - shooter->pos);
		//cam.Commit();

		////// tiled map data orientation == right down, need flip y
		////constexpr float scale = 0.5;
		//float tw = tm.tileWidth * cam.scale.x, th = tm.tileHeight * cam.scale.y;
		//float tmw = tm.width, tmh = tm.height;
		////XY basePos{ gDesign.x7, gDesign.y7 - th };	// -th for flip y

		//auto yFrom = cam.rowFrom;
		//auto yTo = cam.rowTo;
		//auto xFrom = cam.columnFrom;
		//auto xTo = cam.columnTo;

		//for (int y = yFrom; y < yTo; ++y) {
		//	for (int x = xFrom; x < xTo; ++x) {
		//		if (auto&& info = tm.GetGidInfo(layerBG, y, x)) {
		//			auto&& pos = cam.at.Apply({ x * tw, -y * th });
		//			q.SetPosition(pos).SetFrame(info->GetFrame()).Draw();
		//		}
		//	}
		//}
		//for (int y = 0; y < (int)tmh; ++y) {
		//	for (int x = 0; x < (int)tmw; ++x) {
		//		if (auto&& info = tm.GetGidInfo(layerTrees, y, x)) {
		//			printf("%d %d \n", y, x);
		//			q.SetPosition(basePos + XY{ x * tw, -y * th }).SetFrame(info->GetFrame()).Draw();
		//		}
		//	}
		//}

		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
