#pragma once
#include <game_looper.h>

struct Cfg {
	static constexpr int physCellSize{ 32 };	// need >= max monster size
	static constexpr int physNumRows{ 512 };
	static constexpr int physNumCols{ 512 };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapCenterPos{ mapSize.x / 2, mapSize.y / 2 };

	static constexpr float mouseHitRange{ 50.f };
};
inline Cfg gCfg;

struct SceneTest1;
using SceneItem = SceneItemBase<SceneTest1>;

struct PhysSceneItem : SceneItem, SpaceGridCItem<PhysSceneItem, XY> {
	void PhysAdd();				// call after fill pos
	void PhysUpdate();			// call after fill pos
	void PhysRemove();
	bool PhysExists();
	~PhysSceneItem();

	//virtual bool Hit(SceneItem& attacker, int damage);
};

struct SnakeBody : PhysSceneItem {
	static constexpr int cTypeId{ 1 };

	static constexpr float cScale{ 1 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	//static constexpr float cDistance{ 3.f };	// for slime
	static constexpr float cDistance{ 10.f };
	static constexpr float cMinRadians{ g2PI / 32.f };

	xx::Weak<SnakeBody> head, prev;
	bool isTail{};

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> MainTask2();

	XY GenRndPos(float radius, float safeRadius);

	void Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> head_, xx::Weak<SnakeBody> prev_, bool isTail_ = false);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};

struct SceneTest1 : Scene {
	SpaceGridC<PhysSceneItem, XY> sgcPhysItems;	// must at top
	xx::Shared<Node> rootNode;
	Camera camera;
	ItemManager<SnakeBody> im;

	void CreateSnake(XY const& headPos, int len);

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
