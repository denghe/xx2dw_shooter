#include <pch.h>
#include <all.h>

struct PropertiesPanel : Node {
	virtual void Draw() override {
	}
};

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();


}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
