#include <pch.h>

xx::Task<> GameLooper::MainTask() {
	rn.Emplace()->Init(0, { 0,0 }, { 1,1 }, { 0.5, 0.5 }, 120)
		.AddText(U"asdf")
		.AddText(U"asdf", RGBA8_Red)
		.AddText(U"asdf")
		.AddText(U"asdf", RGBA8_Red)
		.AddText(U"asdf")
		.AddText(U"asdf", RGBA8_Red)
		.AddText(U"asdf")
		.AddText(U"asdf", RGBA8_Red)
		.Commit();

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
	rn->Draw();
}
