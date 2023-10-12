#include "pch.h"

GameLooper gLooper;
int32_t main() {
	gLooper.showFps = false;
	gLooper.Run();
}

xx::Task<> GameLooper::MainTask() {
	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
	Quad().SetFrame(Frame::Create(ctcDefault.texs[0])).SetAnchor({0.5,1}).Draw();
}
