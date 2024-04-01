#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"

ResFrames& gRes(gLooper.res);

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::string picRoot = "d30_res/";
	co_await res.AsyncLoad(picRoot, 1024);

	// ui cfg init
	s9cfg.frame = res.ui_border;
	s9cfg.texScale = { 1, 1 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x55, 0x55, 0x55, 0xff };

	// space data init
	srdd.Init(128, 32);

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
