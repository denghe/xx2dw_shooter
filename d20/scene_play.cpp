#include <pch.h>
#include <all.h>

void ScenePlay::Init() {

	rootNode.Emplace()->Init();

	auto& btn1 = rootNode->MakeChildren<Button>();

	btn1->Init(1, { 0, 100 }, { 0.5f, 0.5f }
		, 2, gLooper.frame_button, { 2,3,2,2 }
		, { 0x5f, 0x15, 0xd9, 0xff }
		, U"Exit Game");

	btn1->onClicked = [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	};
}

void ScenePlay::Draw() {
	gLooper.DrawNode(rootNode);
};
