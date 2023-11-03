#include <pch.h>
#include <all.h>

struct SVContent : Node {
	virtual void Draw() override {
		Quad().SetFrame(gLooper.frame_no).SetScale(worldScale * 2)
			.SetPosition(worldMinXY)
			.SetAnchor({})
			.Draw()
			.SetPosition(worldMinXY + XY{ 0, worldSize.y })
			.SetAnchor({ 0, 1 })
			.Draw()
			.SetPosition(worldMinXY + XY{ worldSize.x, worldSize.y })
			.SetAnchor({ 1, 1 })
			.Draw()
			.SetPosition(worldMinXY + XY{ worldSize.x, 0 })
			.SetAnchor({ 1, 0 })
			.Draw()
			;
	}
};

struct SVContentBag : Node {
	static constexpr Vec2<> itemSize{ 16, 16 };	// todo: padding
	static constexpr XY itemSizef{ 16, 16 };

	ScrollView* sv{};
	ItemContainer* container{};
	xx::List<xx::Ref<Frame>, int32_t>* frames{};
	int32_t numCols{}, numRows{};

	void Init(ScrollView* sv_, ItemContainer* container_) {
		Node::Init(sv_->z + 1, {}, { 1,1 }, {}, sv_->size);

		sv = sv_;
		container = container_;
		frames = &gLooper.frames_cheses_1;

		UpdateSize();
	}

	void UpdateSize() {
		// calc bag grid size
		auto& items = container->items;
		numCols = (int32_t)sv->size.x / itemSize.x;
		numRows = items.len / numCols;
		if (numRows * numCols < items.len) ++numRows;
		size = { float(numCols * itemSize.x), float(numRows * itemSize.y) };
		sv->InitContentSize<false>(size);
	}

	virtual void Draw() override {
		// calculate row cut range
		auto rowIdxBegin = int32_t(size.y + parent->position.y - sv->size.y) / itemSize.y;
		auto rowIdxEnd = rowIdxBegin + (int32_t)sv->size.y / itemSize.y + 1;

		auto& items = container->items;
		Quad q;
		auto basePos = worldMinXY;
		for (int32_t rowIdx = rowIdxBegin; rowIdx <= rowIdxEnd; ++rowIdx) {
			for (int32_t colIdx = 0; colIdx < numCols; ++colIdx) {
				auto itemIndex = rowIdx * numCols + colIdx;
				if (itemIndex >= items.len)
					return;
				auto& item = items[itemIndex];
				auto& frame = frames->At(item->typeId);
				XY pos{ colIdx * itemSizef.x + itemSizef.x / 2, size.y - rowIdx * itemSizef.y - itemSizef.y / 2 };
				XY scale{ itemSizef.x / frame->textureRect.w, itemSizef.y / frame->textureRect.h };
				q.SetFrame(frame).SetScale(scale).SetPosition(basePos + pos).Draw();
			}
		}
	}
};

void SceneMainMenu::Init() {

	rootNode.Emplace()->Init();

	{
		auto sv = rootNode->MakeChildren<ScrollView>();
		sv->Init(2, { 50, 50 }, { 1, 1 }, {}, { 200, 200 }, { 50, 50 });

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
		sv->Init(2, { 50, 300 }, { 1, 1 }, {}, { 200, 200 }, { 1, 1 });

		sv->MakeChildren<Scale9Sprite>()->Init(1, {}, { 1,1 }, {}, sv->size, gLooper.s9cfg_panel);
		sv->MakeChildren<SVContent>()->Init(3, {}, { 1,1 }, {}, sv->size);

		XY maxXY{};
		for (size_t i = 0; i < 1000; i++) {
			if (i) maxXY.y -= 3;	// margin
			auto&& btn = sv->MakeContent<Button>();
			btn->Init(4, { 0,maxXY.y }, { 0,1 }, gLooper.s9cfg_btn, xx::StringU8ToU32("btn " + std::to_string(i)));
			if (btn->size.x > maxXY.x) maxXY.x = btn->size.x;
			maxXY.y -= btn->size.y;
		}
		maxXY.y = -maxXY.y;
		for (auto& n : sv->children[0]->children) {
			n->position.y += maxXY.y;
		}

		sv->InitContentSize(maxXY);
	}

	{
		player1.Emplace();

		auto sv = rootNode->MakeChildren<ScrollView>();
		sv->Init(2, { 300, 50 }, { 1, 1 }, {}, { 800, 700 }, { 1, 1 });
		sv->MakeChildren<Scale9Sprite>()->Init(1, { -5, -5 }, { 1,1 }, {}, sv->size + XY{ 10,10 }, gLooper.s9cfg_panel);
		auto&& bagNode = sv->MakeContent<SVContentBag>();
		bagNode->Init(sv, &player1->bag);

		tasks.Add([this, bagNode = bagNode]()->xx::Task<> {
			auto& frames = gLooper.frames_cheses_1;
			for (int32_t i = 0; i < 503; i++) {
				for (int32_t j = 0; j < 9; j++) {
					auto item = xx::MakeShared<ItemBase>();
					item->typeId = gLooper.rnd.Next<int32_t>(frames.len - 1);
					player1->bag.Add(std::move(item));
					bagNode->UpdateSize();
				}
				co_yield 0;
			}
			});
	}

}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
