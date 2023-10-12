#include "pch.h"

GameLooper gLooper;
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_desktop";
	gLooper.Run();
}

xx::Task<> GameLooper::MainTask() {
	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
}
