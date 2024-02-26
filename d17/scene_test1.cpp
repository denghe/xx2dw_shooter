#include <pch.h>
#include <all.h>

#pragma region Bag

XY Bag::GetDrawSize() const {
	return cellSize * XY{ (float)numCols, (float)numRows };
}

xx::Weak<BagItem>& Bag::RefItem(int rowIdx_, int colIdx_) const {
	assert(rowIdx_ >= 0 && rowIdx_ < numRows);
	assert(colIdx_ >= 0 && colIdx_ < numCols);
	return (xx::Weak<BagItem>&)cells[rowIdx_ * numCols + colIdx_];
}

void Bag::Init(int numRows_, int numCols_, XY const& pos_, XY const& cellSize_, XY const& anchor_) {
	numRows = numRows_;
	numCols = numCols_;
	pos = pos_;
	cellSize = cellSize_;
	anchor = anchor_;
	auto total = numRows * numCols;
	items.Reserve(total);
	cells.Resize(total);
}

void Bag::Update() {
	// todo: handle mouse drag?
	for (auto& o : items) {
		o->Update();
	}
}

void Bag::Draw(Camera const& camera) {
	basePos = pos - anchor * GetDrawSize();

	// ready to draw background grid lines
	Quad q;
	q.SetFrame(gLooper.frame_dot_1_22)
		.SetColor(RGBA8_White)
		.SetAnchor({ 0, 1 });

	// draw all horizontal lines
	q.SetScale({ camera.scale * 0.5f * cellSize.x * numCols, 0.5f });
	for (int i = 0; i <= numRows; i++) {
		q.SetPosition(camera.ToGLPos(basePos + XY{ 0, cellSize.y * i } *camera.scale))
			.Draw();
	}
	// draw all vertical lines
	q.SetScale({ 0.5f, camera.scale * 0.5f * cellSize.y * numRows });
	for (int i = 0; i <= numCols; i++) {
		q.SetPosition(camera.ToGLPos(basePos + XY{ cellSize.x * i, 0 } *camera.scale))
			.Draw();
	}

	// draw items
	// todo: in area check ? in scroll view ? cut ?
	for (int y = 0; y < numRows; y++) {
		for (int x = 0; x < numCols; x++) {
			if (auto& c = RefItem(y, x)) {
				c->Draw(camera);
			}
		}
	}
}

#pragma endregion

#pragma region BagItem

void BagItem::BagItemInit(Bag* bag_, int rowIdx_, int colIdx_) {
	assert(bag_);
	bag = bag_;
	assert(!bag->RefItem(rowIdx_, colIdx_));
	bagItemsIndex = bag->items.len;
	bagRowIdx = rowIdx_;
	bagColIdx = colIdx_;
	auto&& self = xx::SharedFromThis(this);
	bag->RefItem(rowIdx_, colIdx_) = self;
	bag->items.Emplace(std::move(self));
}

XY BagItem::GetDrawBasePos() {
	return bag->basePos + bag->cellSize * bag->scale * XY{ (float)bagColIdx, (float)bagRowIdx };
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ gCfg.cellSize, gCfg.cellSize });
	camera.SetOriginal(gCfg.gridCenterPos);
	camera.SetScale(1.f);

	bag.Init(20, 30, { gCfg.gridCenterPos }, { 32, 32 }, { 0.5f, 0.5f });
	// fill some item
	bag.items.Emplace().Emplace<Potion>()->BagItemInit(&bag, 1, 2);
}

void SceneTest1::Update() {
	camera.Calc();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	bag.Update();
}

void SceneTest1::Draw() {
	camera.Calc();
	bag.Draw(camera);
};

#pragma endregion

#pragma region Potion

void Potion::Update() {
	// ??
}

void Potion::Draw(Camera const& camera) {
	Quad().SetFrame(gLooper.frames_potion_4[0])
		.SetScale(camera.scale * bag->scale)
		.SetPosition(camera.ToGLPos(GetDrawBasePos()))
		.Draw();
}

#pragma endregion
