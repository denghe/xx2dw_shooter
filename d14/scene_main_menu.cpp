#include <pch.h>
#include <all.h>

struct SVContent : Node {
	virtual void Draw() override {
		auto basePos = WorldMinXY();
		Quad().SetFrame(gLooper.frame_no).SetScale(WorldScale() * 2)
		.SetPosition(basePos)
		.SetAnchor({})
		.Draw()
		.SetPosition(basePos + XY{ 0, worldSize.y })
		.SetAnchor({0, 1})
		.Draw()
		.SetPosition(basePos + XY{ worldSize.x, worldSize.y })
		.SetAnchor({1, 1})
		.Draw()
		.SetPosition(basePos + XY{ worldSize.x, 0 })
		.SetAnchor({1, 0})
		.Draw()
		;
	};
};

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto sv = rootNode->MakeChildren<ScrollView>();
	sv->Init(3, { -100, 100 }, { 200, 200 }, { 0.5f, 0.5f }, {2, 2});

	sv->MakeChildren<Scale9Sprite>()->Init(1, {}, {}, sv->size, gLooper.s9cfg_panel);
	sv->MakeChildren<SVContent>()->Init(2, {}, sv->size, {});

	sv->MakeContent<RichLabel>()->Init(4, {}, { 1,1 }, {}, sv->size.x)
		.AddText(U" asdfasdfasd f sdf sadf sdf sd fs adf asdf sa fds df s df.\n")
		.Commit();
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
