#include <pch.h>
#include <all.h>

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
