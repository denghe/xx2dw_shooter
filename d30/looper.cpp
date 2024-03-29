#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"

ResFrames& gRes(gLooper.res);

void Looper::ImGuiInit() {
	auto&& io = ImGui::GetIO();
	io.Fonts->ClearFonts();

	//ImFontConfig cfg;
	//cfg.SizePixels = 26.f;
	//auto&& imfnt = io.Fonts->AddFontDefault(&cfg);
	auto&& imfnt = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 24, {}, io.Fonts->GetGlyphRangesChineseFull());

	io.Fonts->Build();
	io.FontDefault = imfnt;

	io.IniFilename = nullptr;
}

xx::Task<> Looper::MainTask() {

	// begin load / download textures
	std::string picRoot = "map_td3/pics/";
	co_await res.AsyncLoad(picRoot, 1024);

	// ui cfg init
	s9cfg.frame = res.td_ui_border;
	s9cfg.texScale = { 1, 1 };
	s9cfg.center = { 1, 1, 1, 1 };
	s9cfg.color = { 0x55, 0x55, 0x55, 0xff };

	// load first scene
	co_await AsyncSwitchTo<SceneMainMenu>();
}
