#include "pch.h"

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();

	// preload texs
    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");
	xx_assert(tp);

	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");			// ready flag

	// load tiled map data
	auto sd = co_await AsyncDownloadFromUrl("res/m.bmx");
	xx_assert(sd);

	// data fill to tiled map
	xx::TmxData td;
	td = std::move(*sd);
	auto r = td.Read(tiledMap);
	xx_assert(!r);
	td.Clear(true);

	// download tiled map's textures
	auto n = tiledMap.images.size();
	for (auto& img : tiledMap.images) {
		tasks.Add([this, &n, img = img, url = std::string("res/") + img->source]()->xx::Task<> {
			img->texture = co_await AsyncLoadTextureFromUrl(url.c_str());
			--n;
			printf("url loaded: %s\n", url.c_str());
		});
	}
	while (n) co_yield 0;	// wait all pic download

	// generate gid cache data
	tiledMap.FillExts();

	layerBG = (TMX::Layer_Tile*)tiledMap.FindLayer("bg");
	layerTrees = (TMX::Layer_Tile*)tiledMap.FindLayer("trees");

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
		for (auto& a : tiledMap.anims) {
			a->Update(delta);
		}

		constexpr float scale = 1;
		Quad q;
		q.SetScale(scale).SetAnchor({0, 0});
		//XY mapSize{ (float)tiledMap.tileHeight * tiledMap.height, (float)tiledMap.tileWidth * tiledMap.width };
		XY basePos{ -gDesign.width_2, -gDesign.height_2 };

		for (uint32_t y = 0; y < tiledMap.height; ++y) {
			for (uint32_t x = 0; x < tiledMap.width; ++x) {
				if (auto info = tiledMap.GetGidInfo(layerBG, y, x)) {
					q.SetPosition(basePos + XY{ (float)x * tiledMap.tileWidth * scale, (float)y * tiledMap.tileHeight * scale }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}
		for (uint32_t y = tiledMap.height - 1; y != -1 ; --y) {
			for (uint32_t x = 0; x < tiledMap.width; ++x) {
				if (auto info = tiledMap.GetGidInfo(layerTrees, y, x)) {
					q.SetPosition(basePos + XY{ (float)x * tiledMap.tileWidth * scale, (float)y * tiledMap.tileHeight * scale }).SetFrame(info->GetFrame()).Draw();
				}
			}
		}

		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}
