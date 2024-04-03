#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"

Looper gLooper;

int32_t main() {
	SetConsoleOutputCP(CP_UTF8);
	xx::SQLite::Init();

	gLooper.showFps = false;
	gLooper.title = "xx2dw_tool_resmgr";
	gLooper.Init();
	gLooper.Run<true>();
}

xx::Task<> Looper::DrawTask() {

	// load first scene
	co_await AsyncSwitchTo<Scene_MainMenu::Scene>();
}
