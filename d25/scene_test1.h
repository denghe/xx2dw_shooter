#pragma once
#include <game_looper.h>

// tower defense: circle world game play ? all of them ar clecles
// monster = 1 ~ n circle, cannon too
// circle texture size: 32x32
// map design: tile set = 32x32, window size = 1280x720, map rows = 22, cols = 40

struct Cfg {
	static constexpr float unitSize{ 32 };

	static constexpr int gridCellSize{ 32 };	// need >= max unit size
	static constexpr int gridNumRows{ 40 };
	static constexpr int gridNumCols{ 22 };

	static constexpr XY mapSize{ float(gridNumCols * gridCellSize), float(gridNumRows * gridCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
};
inline Cfg gCfg;

struct SceneTest1;
inline SceneTest1* gScene;		// init by SceneTest1::Init()

struct MonsterBase : GridItemBase {
	static constexpr double cHP{ 100 };
	static constexpr float cRadius{ 16 };
	static constexpr float cSpeed{ 10 };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double hp{};					// fill by init
	float radius{};
	int trackIndex{};				// random by "safe range"
	float pointIndex{}, speed{}, radians{};
	// todo: more?
};

struct Monster1 : MonsterBase {
	static constexpr uint32_t cTypeId{ 0 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	double hpBak{};

	void Init(double hp_);
	bool Update();
	void Draw();
};

// todo: Map, Tower

struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	Rnd rnd;
	TrackManager tm;
	Grid<Monster1> grid;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
