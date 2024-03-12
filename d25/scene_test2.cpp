#include "pch.h"
#include "cfg.h"
#include "scene_test2.h"
#include "scene_main_menu.h"

#pragma region SceneTest2

void SceneTest2::Init() {
	gSceneEditor = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	// ...

	// todo: fill map context. layers name: platforms blocks path1...

	map = gLooper.map2;
	auto w = map->width;
	auto h = map->height;
	auto e = int(w * h);
	mapFrames.Resize(w * h);

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

	// todo: calculate path ?
	// search start pos
	// store path gid


	static constexpr auto prefix = "td_path_"sv;
	static constexpr auto suffix = ".png"sv;
	for (auto& gi : map->gidInfos) {
		if (!gi) continue;
		if (gi.image->source.starts_with(prefix)) {
			std::string_view sv(gi.image->source.c_str() + prefix.size(), gi.image->source.size() - prefix.size() - suffix.size());
			sv.size();
		}
	}

	camera.SetScale(2.f);
	camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	camera.SetMaxFrameSize({ (float)gCfg.gridCellSize, (float)gCfg.gridCellSize });

}

void SceneTest2::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	// todo
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

	// todo

	gLooper.DrawNode(rootNode);
}

#pragma endregion
