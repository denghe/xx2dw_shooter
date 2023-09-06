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

	shooter.Emplace()->Init();	// make player plane

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

		constexpr float scale = 1;
		Quad q;
		q.SetScale(scale).SetAnchor({0, 0});
		//XY mapSize{ (float)tm.tileHeight * tm.height, (float)tm.tileWidth * tm.width };
		XY basePos{ -gDesign.width_2, -gDesign.height_2 };

		for (uint32_t y = 0; y < tm.height; ++y) {
			for (uint32_t x = 0; x < tm.width; ++x) {
				if (auto info = tm.GetGidInfo(layerBG, y, x)) {
					q.SetPosition(basePos + XY{ (float)x * tm.tileWidth * scale, (float)y * tm.tileHeight * scale }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}
		for (uint32_t y = tm.height - 1; y != -1 ; --y) {
			for (uint32_t x = 0; x < tm.width; ++x) {
				if (auto info = tm.GetGidInfo(layerTrees, y, x)) {
					q.SetPosition(basePos + XY{ (float)x * tm.tileWidth * scale, (float)y * tm.tileHeight * scale }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}

		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
