#pragma once
#include <game_looper.h>

// simulate bag -> item drag for move & swap, sort

// todo:
// number ? star ? new ?
// mouse down a little while begin drag ? or mouse down + move begin drag ? mouse down + up = click ? set focus ? context menu ? hover info panel ? 
// event sound effect ?

struct Item {
	int typeId{};	// need fill by Init
	XY pos{}, anchor{ 0.5f, 0.5f };
	float scale{ 1 };
	bool dragging{};
	virtual void Update(Camera const& camera) {};
	virtual void Draw(Camera const& camera) {};
};

struct Bag;
struct BagItem : Item {
	xx::Weak<Bag> bag;
	int bagItemsIndex{}, bagRowIdx{}, bagColIdx{};
	void BagItemInit(xx::Weak<Bag> bag_, int rowIdx_, int colIdx_);	// add item to items & cells
	XY GetDrawBasePos();
};

struct Bag : Item {
	xx::Listi32<xx::Shared<BagItem>> items;		// todo: group? quantity combine?
	xx::Listi32<xx::Weak<BagItem>> cells;

	int numRows{}, numCols{};
	XY cellSize{};

	XY basePos{};	// fill when Draw begin

	XY dragPos;		// last mouse pos
	xx::Weak<BagItem> dragItem;

	xx::Weak<BagItem>& RefCell(int rowIdx_, int colIdx_) const;
	XY GetDrawSize() const;
	Vec2<> PosToCellIndex(XY const& pos_) const;	// out of range: return -1, -1
	xx::Weak<BagItem> GetItemByPos(XY const& pos_) const;
	void Sort();

	void Init(int numRows_, int numCols_, XY const& cellSize_, XY const& pos_, XY const& anchor_);
	virtual void Update(Camera const& camera) override;
	virtual void Draw(Camera const& camera) override;
};

struct SceneTest1 : Scene {
	Camera camera;
	xx::Shared<Bag> bag;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};

struct Potion : BagItem {
	int frameIndex{};
	void Init(xx::Weak<Bag> bag_, int rowIdx_, int colIdx_);
	virtual void Update(Camera const& camera) override;
	virtual void Draw(Camera const& camera) override;
};
