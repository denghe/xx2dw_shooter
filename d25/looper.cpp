#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "map_path.h"

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::string resRoot = "map_td3/";
	std::string picRoot = resRoot + "pics/";

	// prepare
	frames_number_outlined.Resize(10);
	ffs.emplace_back(picRoot + "number_outlined_0.png", &frames_number_outlined[0]);
	ffs.emplace_back(picRoot + "number_outlined_1.png", &frames_number_outlined[1]);
	ffs.emplace_back(picRoot + "number_outlined_2.png", &frames_number_outlined[2]);
	ffs.emplace_back(picRoot + "number_outlined_3.png", &frames_number_outlined[3]);
	ffs.emplace_back(picRoot + "number_outlined_4.png", &frames_number_outlined[4]);
	ffs.emplace_back(picRoot + "number_outlined_5.png", &frames_number_outlined[5]);
	ffs.emplace_back(picRoot + "number_outlined_6.png", &frames_number_outlined[6]);
	ffs.emplace_back(picRoot + "number_outlined_7.png", &frames_number_outlined[7]);
	ffs.emplace_back(picRoot + "number_outlined_8.png", &frames_number_outlined[8]);
	ffs.emplace_back(picRoot + "number_outlined_9.png", &frames_number_outlined[9]);

	ffs.emplace_back(picRoot + "td_ui_border.png", &frame_td_ui_border);
	ffs.emplace_back(picRoot + "td_ui_star.png", &frame_td_ui_star);
	ffs.emplace_back(picRoot + "td_ui_gem.png", &frame_td_ui_gem);
	ffs.emplace_back(picRoot + "td_ui_gear.png", &frame_td_ui_gear);
	ffs.emplace_back(picRoot + "td_ui_coin.png", &frame_td_ui_coin);
	ffs.emplace_back(picRoot + "td_ui_menu.png", &frame_td_ui_menu);
	ffs.emplace_back(picRoot + "td_ui_pause.png", &frame_td_ui_pause);
	ffs.emplace_back(picRoot + "td_ui_run1.png", &frame_td_ui_run1);
	ffs.emplace_back(picRoot + "td_ui_run2.png", &frame_td_ui_run2);
	ffs.emplace_back(picRoot + "td_ui_run3.png", &frame_td_ui_run3);
	ffs.emplace_back(picRoot + "td_ui_clock.png", &frame_td_ui_clock);
	ffs.emplace_back(picRoot + "td_ui_aim.png", &frame_td_ui_aim);
	ffs.emplace_back(picRoot + "td_ui_heart.png", &frame_td_ui_heart);
	ffs.emplace_back(picRoot + "td_ui_sword.png", &frame_td_ui_sword);
	ffs.emplace_back(picRoot + "td_cell_gold_mine.png", &frame_td_cell_gold_mine);
	ffs.emplace_back(picRoot + "td_icon_sun.png", &frame_td_icon_sun);
	ffs.emplace_back(picRoot + "td_cell_water.png", &frame_td_cell_water);
	ffs.emplace_back(picRoot + "td_icon_coin.png", &frame_td_icon_coin);
	ffs.emplace_back(picRoot + "td_cell_road.png", &frame_td_cell_road);
	ffs.emplace_back(picRoot + "td_icon_cannon3.png", &frame_td_icon_cannon3);
	ffs.emplace_back(picRoot + "td_icon_cannon1.png", &frame_td_icon_cannon1);
	ffs.emplace_back(picRoot + "td_icon_arrow3.png", &frame_td_icon_arrow3);
	ffs.emplace_back(picRoot + "td_icon_arrow1.png", &frame_td_icon_arrow1);
	ffs.emplace_back(picRoot + "td_tower_base.png", &frame_td_tower_base);
	ffs.emplace_back(picRoot + "td_cell_mouse_focus.png", &frame_td_cell_mouse_focus);
	ffs.emplace_back(picRoot + "td_cell_montain.png", &frame_td_cell_montain);
	ffs.emplace_back(picRoot + "td_cell_space.png", &frame_td_cell_space);
	ffs.emplace_back(picRoot + "td_cell_empty.png", &frame_td_cell_empty);
	ffs.emplace_back(picRoot + "td_shape_dot.png", &frame_td_shape_dot);
	ffs.emplace_back(picRoot + "td_shape_trangle.png", &frame_td_shape_trangle);
	ffs.emplace_back(picRoot + "td_shape_circle.png", &frame_td_shape_circle);

	// load / download
	for (auto& ff : ffs) {
#ifdef __EMSCRIPTEN__
		*ff.second = co_await AsyncLoadFrameFromUrl(ff.first);
#else
		*ff.second = LoadFrame(ff.first);
#endif
	}

	// batch combine textures
	auto ok = DynamicTexturePacker<512>::Pack(ffs);
	assert(ok);


	// load tiled map data. layer names:  map,  fg1, fg2,  path

#ifdef __EMSCRIPTEN__
	map_stage1 = co_await AsyncLoadTiledMapFromUrl<true>(resRoot + "stage1.bmx");
	map_stage2 = co_await AsyncLoadTiledMapFromUrl<true>(resRoot + "stage2.bmx");
#else
	map_stage1 = LoadTiledMap<true>(resRoot + "stage1.bmx");
	map_stage2 = LoadTiledMap<true>(resRoot + "stage2.bmx");
#endif

	// mapping frames to gidInfo
	// todo

	std::unordered_map<std::string, xx::Ref<Frame>*> ffm;
	for (auto& o : ffs) {
		ffm.emplace(o.first.substr(picRoot.size()), o.second);
	}

	for (auto& gi : map1->gidInfos) {
		if (!gi) continue;
		auto it = ffm.find(gi.image->source);
		assert(it != ffm.end());
		gi.frame = *it->second;
	}

	//MapPath::InitMapGidInfos(map2);

	// phys init
	sgrdd.Init(128, 32);

	// ui cfg init
	s9cfg.frame = frame_td_ui_border;
	s9cfg.texScale = { 2, 2 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x5f, 0x15, 0xd9, 0xff };

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
