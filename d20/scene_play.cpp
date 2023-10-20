﻿#include <pch.h>
#include <all.h>

void ScenePlay::Init() {

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, { 0, 200 }, { 0.5f, 0.5f }
		, 2, gLooper.frame_button, { 2,3,2,2 }, { 0x5f, 0x15, 0xd9, 0xff }
		, U"Exit Game"
		, [&]() {
			gLooper.DelaySwitchTo<SceneMainMenu>();
		});
}

void ScenePlay::Draw() {
	gLooper.DrawNode(rootNode);
};
