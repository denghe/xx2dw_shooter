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
		camera.DecreaseScale(0.25, 0.25);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.2)) {
		camera.IncreaseScale(0.25, 10);
	}
	if (!ready) return;

	shooter();
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {

	if (ready) {
		camera.SetOriginal(shooter->pos);
		camera.Calc(gMaxFramePixelWidth, gMaxFramePixelHeight);

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
		
		int maxTreeY = tm.GetMaxRowIndex(camera, 2);
		for (int y = minY; y < maxTreeY; ++y) {
			for (int x = minX; x < maxX; ++x) {
				if (auto&& info = tm.GetGidInfo(layerTrees, y, x)) {
					q.SetPosition(basePos + XY::Make(x, -y) * scaledTileSize).TrySetFrame(info->GetFrame()).Draw();
				}
			}
		}

		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
