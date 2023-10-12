#include "pch.h"

GameLooper gLooper;
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_desktop";
	gLooper.Init();
	gLooper.Run();
}

// todo: file system  picture load ...

xx::Task<> GameLooper::MainTask() {
	frame_button = LoadSharedFrame("res/button.png");

	//root.Emplace()->scale = { 0.45f, 0.45f };
	root.Emplace()->scale = { 3, 3 };
	root->FillTrans();

	//for (size_t i = 0; i < 10000; i++) {
	//	XY pos{ rnd.Next<float>(-width, width), rnd.Next<float>(-height, height) };
	//	RGBA8 color;
	//	(uint32_t&)color = rnd.Next<uint32_t>();
	//	color.a = 255;
	//	auto txt = xx::StringU8ToU32(rnd.NextWord());
	//	root->MakeChildren<Button>()->Init(1, pos, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, color, txt);
	//}

	root->MakeChildren<Button>()->Init(1, { 0, -50 }, {0.5, 0.5}, buttonTextureScale, frame_button, buttonUvRect, { 0x5f, 0x15, 0xd9, 0xff }, U"asd👻🎃fqwer");
	root->MakeChildren<Button>()->Init(1, {}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0x2b, 0x39, 0xfb, 0xff }, U"ASADF");
	root->MakeChildren<Button>()->Init(1, {0, 50}, { 0.5, 0.5 }, buttonTextureScale, frame_button, buttonUvRect, { 0xe7, 0x8d, 0x00, 0xff }, U"zxcv123");

	co_return;
}

void GameLooper::Update() {
}

void GameLooper::Draw() {
	FillZNodes(tmpZNodes, root);
	OrderByZDrawAndClear(tmpZNodes);
	LineStrip().FillCirclePoints({ 0, 0 }, 2, {}, 8).Draw();
	LineStrip().FillCirclePoints({ 0, 120 }, 2, {}, 8).Draw();
}
