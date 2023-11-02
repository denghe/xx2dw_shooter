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

	{
		auto sv = rootNode->MakeChildren<ScrollView>();
		sv->Init(2, { 200, 100 }, { 1, 1 }, {}, { 200, 200 }, { 50, 50 });

		sv->MakeChildren<Scale9Sprite>()->Init(1, {}, { 1,1 }, {}, sv->size, gLooper.s9cfg_panel);
		sv->MakeChildren<SVContent>()->Init(3, {}, { 1,1 }, {}, sv->size);

		auto&& rl = sv->MakeContent<RichLabel>();
		rl->Init(4, {}, { 1,1 }, {}, sv->size.x)
			.AddText(U" asdfasdfasd f sdf sadf sdf sd fs adf asdf sf sdf sadf sdf sd fs adf asdf sa fds df s df.\n");
		rl->Commit();

		sv->InitContentSize(rl->size);
	}


	{
		auto sv = rootNode->MakeChildren<ScrollView>();
		sv->Init(2, { 600, 200 }, { 1, 1 }, {}, { 200, 200 }, { 50, 50 });

		sv->MakeChildren<Scale9Sprite>()->Init(1, {}, { 1,1 }, {}, sv->size, gLooper.s9cfg_panel);
		sv->MakeChildren<SVContent>()->Init(3, {}, { 1,1 }, {}, sv->size);

		XY maxXY{};
		for (size_t i = 0; i < 1000; i++) {
			if (i) maxXY.y -= 3;	// margin
			auto&& btn = sv->MakeContent<Button>();
			btn->Init(4, {0,maxXY.y}, {0,1}, gLooper.s9cfg_btn, xx::StringU8ToU32("btn " + std::to_string(i)));
			if (btn->size.x > maxXY.x) maxXY.x = btn->size.x;
			maxXY.y -= btn->size.y;
		}
		maxXY.y = -maxXY.y;
		for (auto& n : sv->children[0]->children) {
			n->position.y += maxXY.y;
		}

		sv->InitContentSize(maxXY);
	}
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
