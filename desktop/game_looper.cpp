﻿#include "pch.h"

GameLooper gLooper;
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_desktop";
	gLooper.Init();
	gLooper.Run();
}

// todo: file system  picture load ...

xx::Task<> GameLooper::MainTask() {
	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
}
