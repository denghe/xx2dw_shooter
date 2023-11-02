#include <pch.h>
#include <all.h>

struct SVContent : Node {
	virtual void Draw() override {
		Quad().SetFrame(gLooper.frame_no).SetScale(worldScale * 2)
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
	sv->Init(2, { 200, 100 }, { 1, 1 }, {}, { 200, 200 }, { 50, 50 });

	sv->MakeChildren<Scale9Sprite>()->Init(1, {}, {1,1}, {}, sv->size, gLooper.s9cfg_panel);
	sv->MakeChildren<SVContent>()->Init(3, {}, { 1,1 }, {}, sv->size);

	auto&& rl = sv->MakeContent<RichLabel>();
	rl->Init(4, {}, { 1,1 }, {}, sv->size.x)
		.AddText(U" asdfasdfasd f sdf sadf sdf sd fs adf asdf sf sdf sadf sdf sd fs adf asdf sa fds df s df.\n");
	rl->Commit();
	
	sv->InitContentSize(rl->size);
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
