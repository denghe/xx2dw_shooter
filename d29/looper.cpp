#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::string picRoot = "map_td3/pics/";
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	ffs.emplace_back(picRoot + "td_ui_border.png", &frame_td_ui_border);
	ffs.emplace_back(picRoot + "td_effect_1.png", &frame_td_effect_1);
	ffs.emplace_back(picRoot + "td_shape_dot.png", &frame_td_shape_dot);
	ffs.emplace_back(picRoot + "td_shape_ring.png", &frame_td_shape_ring);
	ffs.emplace_back(picRoot + "td_shape_rect.png", &frame_td_shape_rect);
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

	// ui cfg init
	s9cfg.frame = frame_td_ui_border;
	s9cfg.texScale = { 1, 1 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x55, 0x55, 0x55, 0xff };

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
