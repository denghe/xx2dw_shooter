#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, { 0, 200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Exit Game", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});
}

void ScenePlay::Draw() {
	gLooper.DrawNode(rootNode);
};
