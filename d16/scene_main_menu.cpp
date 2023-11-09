#include <pch.h>
#include <all.h>

struct Node1 : Node {
	virtual void Draw() override {
		LineStrip().FillCirclePoints({}, 5).SetColor(RGBA8_Green).Draw();
	}
};
struct Node2 : Node {
	virtual void Draw() override {
		LineStrip().FillCirclePoints({}, 10).SetColor(RGBA8_Red).Draw();
	}
};

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Label>()->Init(1, {50,50}, { 1,1 }, { 0.5f,0.5f }, RGBA8_White, "asdf");
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
