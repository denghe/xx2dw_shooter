#include <pch.h>

xx::Task<> GameLooper::MainTask() {
	rn.Emplace()->Init(0, { 0,0 }, { 1,1 }, { 0, 1 }, 100)
		.AddSpace({ 48, 24 })
		.AddText(Aligns::)
		;

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
}
