#pragma once
#include <game_looper.h>

// todo: simulate bag -> equip item drag

struct Cfg {
	static constexpr float cellSize{ 32 };

	static constexpr int numRows{ 512 };
	static constexpr int numCols{ 512 };

	static constexpr XY gridSize{ numCols * cellSize, numRows * cellSize };
	static constexpr XY gridSize_2{ gridSize.x / 2, gridSize.y / 2 };
	static constexpr XY gridCenterPos{ gridSize_2 };

	static constexpr float mouseHitRange{ 50.f };
};
inline Cfg gCfg;

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
	// todo: Init()
	virtual void Update() override;
	virtual void Draw(Camera const& camera) override;
};
