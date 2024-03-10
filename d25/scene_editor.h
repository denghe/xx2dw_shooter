#pragma once
#include "looper.h"

// tower defense: map editor

struct SceneEditor;
inline SceneEditor* gSceneEditor;		// init by SceneEditor::Init()

// todo

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

struct SceneEditor : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
