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
		});
	}
	while (n) co_yield 0;	// wait all pic download

	// generate gid cache data
	tiledMap.FillGidInfos();

	//std::vector<TMX::Layer_Tile*> lts;
	//TMX::FillTo(lts, tiledMap.layers);

	//layerBG.layer = xx::TMX::FindLayer(lts, "bg"sv);
	//assert(layerBG.layer);
	//layerTrees.layer = xx::TMX::FindLayer(lts, "trees"sv);
	//assert(layerTrees.layer);


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


		shooter->Draw();
		bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

		// todo: more Draw

	}
	fv.Draw(ctc72);       // draw fps at corner
}

