#include <pch.h>

xx::Task<> GameLooper::MainTask() {
	rn.Emplace()->Init(0, { 0,0 }, { 1,1 }, { 0, 1 }, 100)
		.AddText(VAligns::Center, RGBA8_White, U"asdf")
		;

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
}
