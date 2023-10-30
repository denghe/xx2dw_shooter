#include <pch.h>
#include <all.h>

struct SVContent : Node {

	XY basePos{}, scaledSize{};

	// todo: args: scroll view ?
	void Init(int z_, XY const& position_, XY const& size_, XY const& anchor_ = { 0.5f, 0.5f }, XY const& scale_ = { 1,1 }) {
		z = z_;
		position = position_;
		scale = scale_;
		anchor = anchor_;
		size = size_;
		SyncBasePosScaledSize();
	}

	XX_FORCE_INLINE void SyncBasePosScaledSize() {
		Node::FillTrans();
		basePos = trans;
		XY pmax{ trans(size) };
		scaledSize = pmax - basePos;
	}

	virtual void Draw() override {
		XY basePos = trans;
		XY pmax{ trans(size) };
		auto scaledSize = pmax - basePos;
		Quad().SetFrame(gLooper.frame_no).SetScale(XY{trans.a, trans.d} * 2)
		.SetPosition(basePos)
		.SetAnchor({})
		.Draw()
		.SetPosition(basePos + XY{ 0, scaledSize.y })
		.SetAnchor({0, 1})
		.Draw()
		.SetPosition(basePos + XY{ scaledSize.x, scaledSize.y })
		.SetAnchor({1, 1})
		.Draw()
		.SetPosition(basePos + XY{ scaledSize.x, 0 })
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
	sv->MakeChildren<SVContent>()->Init(3, {}, sv->size, {});
	sv->MakeChildrenEnd();
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
