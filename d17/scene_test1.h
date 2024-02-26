#pragma once
#include <game_looper.h>

// todo: simulate bag -> equip item drag

struct Item {
	XY pos{}, anchor{ 0.5f, 0.5f };
	float scale{ 1 };
	virtual void Update() {};
	virtual void Draw(Camera const& camera) {};
};

struct Bag;
struct BagItem : Item {
	Bag* bag{};
	int bagItemsIndex{}, bagRowIdx{}, bagColIdx{};
	void BagItemInit(Bag* bag_, int rowIdx_, int colIdx_);	// add item to items & cells
	XY GetDrawBasePos();
};

struct Bag : Item {
	xx::Listi32<xx::Shared<BagItem>> items;
	xx::Listi32<xx::Weak<BagItem>> cells;
	int numRows{}, numCols{};
	XY cellSize{};
	XY basePos{};	// fill when Draw begin

	xx::Weak<BagItem>& RefItem(int rowIdx_, int colIdx_) const;
	XY GetDrawSize() const;
	void Init(int numRows_, int numCols_, XY const& cellSize_, XY const& pos_, XY const& anchor_);
	virtual void Update() override;
	virtual void Draw(Camera const& camera) override;
};

struct SceneTest1 : Scene {
	Camera camera;
	Bag bag;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};

struct Potion : BagItem {
	int frameIndex{};
	void Init(Bag* bag_, int rowIdx_, int colIdx_);
	virtual void Update() override;
	virtual void Draw(Camera const& camera) override;
};
