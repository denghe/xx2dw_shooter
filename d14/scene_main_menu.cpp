#include <pch.h>
#include <all.h>

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto sv = rootNode->MakeChildren<ScrollView>();
	sv->Init(0, { -100, 100 }, { 200, 200 });
	sv->onDrawContent = [&] {
		Quad().SetFrame(gLooper.frame_no).SetScale(2).Draw();
	};
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
