#include <pch.h>
#include <all.h>

#pragma region Bag

XY Bag::GetDrawSize() const {
	return cellSize * XY{ (float)numCols, (float)numRows };
}

Vec2<> Bag::PosToCellIndex(XY const& pos_) const {
	Vec2<> r{ -1, -1 };
	auto siz = GetDrawSize();
	auto bPos = pos - anchor * siz;
	auto p = pos_ - basePos;
	if (p.x > 0 && p.x < siz.x &&
		p.y > 0 && p.y < siz.y) {
		r.x = int(p.x / cellSize.x);
		r.y = int(p.y / cellSize.y);
	}
	return r;
}

xx::Weak<BagItem> Bag::GetItemByPos(XY const& pos_) const {
	if (auto [colIdx, rowIdx] = PosToCellIndex(pos_); colIdx != -1) {
		return RefCell(rowIdx, colIdx);
	}
	return {};
}

xx::Weak<BagItem>& Bag::RefCell(int rowIdx_, int colIdx_) const {
	assert(rowIdx_ >= 0 && rowIdx_ < numRows);
	assert(colIdx_ >= 0 && colIdx_ < numCols);
	auto& o = cells[rowIdx_ * numCols + colIdx_];
	//assert(!o || o->bagRowIdx == rowIdx_ && o->bagColIdx == colIdx_);
	return (xx::Weak<BagItem>&)o;
}

void Bag::Sort() {
	// sort items memory		// unsafe for avoid + - counter
	std::sort((Potion**)items.buf, (Potion**)items.buf + items.len, [](Potion* const& a, Potion* const& b) {
		return a->frameIndex < b->frameIndex;
		});

	// fill cells
	int i{};
	for (int y = 0; y < numRows; y++) {
		for (int x = 0; x < numCols; x++) {
			auto& o = (void*&)cells[y * numCols + x];
			if (i < items.len) {
				auto& item = items[i];
				o = item.GetHeader();	// unsafe for avoid + - counter
				item->bagColIdx = x;
				item->bagRowIdx = y;
				++i;
			} else {
				memset(&o, 0, sizeof(void*) * (cells.len - i));		// unsafe for avoid + - counter
				return;
			}
		}
	}
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

void Bag::Update(Camera const& camera) {

	// handle mouse drag cell item
	dragPos = camera.ToLogicPos(gEngine->mouse.pos);
	if (gEngine->mouse.btnStates[0]) {
		if (!dragItem) {
			dragItem = GetItemByPos(dragPos);
			if (dragItem) {
				dragItem->dragging = true;
			}
		}
	} else if (dragItem) {
		if (auto [colIdx, rowIdx] = PosToCellIndex(dragPos); colIdx != -1) {
			auto& p = RefCell(dragItem->bagRowIdx, dragItem->bagColIdx);
			if (auto& o = RefCell(rowIdx, colIdx); !o) {
				(void*&)o = std::exchange((void*&)p, nullptr);	// unsafe for avoid + - counter
				dragItem->bagRowIdx = rowIdx;
				dragItem->bagColIdx = colIdx;
			} else if (o != dragItem) {
				std::swap(o->bagRowIdx, p->bagRowIdx);
				std::swap(o->bagColIdx, p->bagColIdx);
				std::swap((void*&)o, (void*&)p);	// unsafe for avoid + - counter
			}
		}
		dragItem->dragging = {};
		dragItem.Reset();
	}

	// handle keyboard "S" for sort
	if (!dragItem && gLooper.KeyDownDelay(KeyboardKeys::S, 0.1f)) {
		Sort();
	}

	for (auto& o : items) {
		o->Update(camera);
	}
}

void Bag::Draw(Camera const& camera) {
	auto siz = GetDrawSize();
	basePos = pos - anchor * siz;

	// ready to draw background grid lines
#if 1
	Quad q;
	q.SetFrame(gLooper.frame_dot_1_22)
		.SetColor(RGBA8_White)
		.SetAnchor({ 0, 1 });

	// draw all horizontal lines
	q.SetScale({ camera.scale * 0.5f * siz.x, 0.5f });
	for (int i = 0; i <= numRows; i++) {
		q.SetPosition(camera.ToGLPos(basePos + XY{ 0, cellSize.y * i }))
			.Draw();
	}
	// draw all vertical lines
	q.SetScale({ 0.5f, camera.scale * 0.5f * siz.y });
	for (int i = 0; i <= numCols; i++) {
		q.SetPosition(camera.ToGLPos(basePos + XY{ cellSize.x * i, 0 }))
			.Draw();
	}
#else
	// draw all horizontal lines
	LineStrip ls;
	auto& ps = ls.SetPoints();
	ps.push_back({ 0, 0 });
	ps.push_back({ camera.scale * siz.x });
	for (int i = 0; i <= numRows; i++) {
		ls.SetPosition(camera.ToGLPos(basePos + XY{ 0, cellSize.y * i }))
			.Draw();
	}

	// draw all vertical lines
	ls.dirty = true;
	ps[1] = { 0, -camera.scale * siz.y };
	for (int i = 0; i <= numCols; i++) {
		ls.SetPosition(camera.ToGLPos(basePos + XY{ cellSize.x * i, 0 }))
			.Draw();
	}
#endif

	// draw items
	// todo: in area check ? in scroll view ? cut ?
	for (int y = 0; y < numRows; y++) {
		for (int x = 0; x < numCols; x++) {
			if (auto& c = RefCell(y, x)) {
				c->Draw(camera);
			}
		}
	}

	// draw dragItem at dragPos
	if (dragItem) {
		auto bak = dragItem->pos;
		dragItem->pos = dragPos;
		dragItem->Draw(camera);
		dragItem->pos = bak;
	}
}

#pragma endregion

#pragma region BagItem

void BagItem::BagItemInit(xx::Weak<Bag> bag_, int rowIdx_, int colIdx_) {
	assert(bag_);
	bag = std::move(bag_);
	assert(!bag->RefCell(rowIdx_, colIdx_));
	bagItemsIndex = bag->items.len;
	bagRowIdx = rowIdx_;
	bagColIdx = colIdx_;
	auto&& self = xx::SharedFromThis(this);
	bag->RefCell(rowIdx_, colIdx_) = self;
	bag->items.Emplace(std::move(self));
}

XY BagItem::GetDrawBasePos() {
	assert(bag);
	if (dragging) return bag->dragPos;
	return bag->basePos + bag->cellSize * XY{ bagColIdx + 0.5f, bagRowIdx + 0.5f };
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {

	camera.SetScale(0.5f);
	bag.Emplace()->Init(42, 78, {}, { 32, 32 }, { 0.5f, 0.5f });

	//camera.SetScale(1.f);
	//bag.Emplace()->Init(21, 39, {}, { 32, 32 }, { 0.5f, 0.5f });

	//camera.SetScale(2.f);
	//bag.Emplace()->Init(10, 19, {}, {32, 32}, {0.5f, 0.5f});

	// fill some item
	for (int y = 0; y < bag->numRows; y++) {
		for (int x = 0; x < bag->numCols; x++) {
			if (gLooper.rnd.Next<bool>()) {
				xx::MakeShared<Potion>()->Init(bag, y, x);
			}
		}
	}
}

void SceneTest1::Update() {
	camera.Calc();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	bag->Update(camera);
}

void SceneTest1::Draw() {
	camera.Calc();
	bag->Draw(camera);
};

#pragma endregion

#pragma region Potion

void Potion::Init(xx::Weak<Bag> bag_, int rowIdx_, int colIdx_) {
	frameIndex = gLooper.rnd.Next(3);
	BagItemInit(std::move(bag_), rowIdx_, colIdx_);
}

void Potion::Update(Camera const& camera) {
	// ??
}

void Potion::Draw(Camera const& camera) {
	Quad().SetFrame(gLooper.frames_potion_4[frameIndex])
		.SetScale(camera.scale)
		.SetPosition(camera.ToGLPos(GetDrawBasePos()))
		.Draw();
}

#pragma endregion
