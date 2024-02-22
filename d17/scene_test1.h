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

struct Bag;
struct BagCell {
	static constexpr XY cellMargin{ 5, 5 };

	void Update(Bag* owner);
	void Draw(Camera const& camera);
};

struct Bag {
	static constexpr XY gridPadding{ 10, 10 };

	XY GetGridSize();

	// todo: grid line & margin cfg
	xx::Listi32<xx::Shared<BagCell>> cells;
	int numRows{}, numCols{};

	void Init(int numRows_, int numCols_);
	void Update();
	void Draw(Camera const& camera);
};

struct SceneTest1 : Scene {
	Camera camera;
	Bag bag;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
