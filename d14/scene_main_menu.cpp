#include <pch.h>
#include <all.h>

struct SVContent : Node {
	virtual void Draw() override {
		Quad().SetFrame(gLooper.frame_no).SetScale(XY{trans.a, trans.d} * 2)
		.SetPosition(worldMinXY)
		.SetAnchor({})
		.Draw()
		.SetPosition(worldMinXY + XY{ 0, worldSize.y })
		.SetAnchor({0, 1})
		.Draw()
		.SetPosition(worldMinXY + XY{ worldSize.x, worldSize.y })
		.SetAnchor({1, 1})
		.Draw()
		.SetPosition(worldMinXY + XY{ worldSize.x, 0 })
		.SetAnchor({1, 0})
		.Draw()
		;
	};
};

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto sv = rootNode->MakeChildren<ScrollView>();
	sv->Init(2, { -100, 100 }, { 200, 200 }, { 0.5f, 0.5f }, {2, 2});
	sv->MakeChildren<Scale9Sprite>()->Init(1, {}, {}, sv->size, gLooper.s9cfg_panel);
	sv->MakeContent<SVContent>()->Init(3, {}, sv->size, {});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
