#pragma once
#include <game_looper.h>

struct Cfg {
	static constexpr float maxItemSize{ 32 };

	static constexpr int physCellSize{ 32 };	// need >= max item size
	static constexpr int physNumRows{ 512 };
	static constexpr int physNumCols{ 512 };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
	static constexpr XY mapCenterPos{ mapSize_2 };
	static constexpr XY mapSafeMinPos{ maxItemSize, maxItemSize };
	static constexpr XY mapSafeMaxPos{ mapSize.x - maxItemSize, mapSize.y - maxItemSize };

	static constexpr float mouseHitRange{ 50.f };
};
inline Cfg gCfg;



struct SceneTest1;
using SceneItem = SceneItemBase<SceneTest1>;

struct SceneTest1;
struct PhysSceneItem : SceneItem, SpaceGridCItem<PhysSceneItem, XY> {
	void PhysAdd();				// call after fill pos
	void PhysUpdate();			// call after fill pos
	void PhysRemove();
	bool PhysExists();
	~PhysSceneItem();
	virtual bool Hit(SceneItem& attacker, int damage);
};



struct Human;
struct Staff : SceneItem {
	static constexpr int cTypeId{ 1 };
	static constexpr XY cAnchor{ 0.2f, 0.5f };
	static constexpr int cFrameIndex{ 1 };

	static constexpr float cFrameMaxChangeRadians{ float(M_PI * 10 / gDesign.fps) };
	static constexpr float cFireDelaySecs{ 0.05f };
	static constexpr float cFireDistance{ 18 };

	xx::Weak<SceneItem> owner;
	float nextFireSecs{};
	XY firePos{};

	xx::Task<> mainTask;
	xx::Task<> MainTask();

	void Init(ItemManagerBase* im_, xx::Weak<SceneItem> owner_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};



struct Bomb : SceneItem {
	static constexpr int cTypeId{ 2 };

	static constexpr float cScale{ 1.f };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	static constexpr float cRadius{ 100 };
	static constexpr float cLifeSpan{ 0.5 };
	static constexpr int cLifeNumFrames{ int(cLifeSpan / gDesign.frameDelay) };

	xx::Weak<SceneItem> owner;
	XY fromPos{}, toPos{}, controlPoint1{}, controlPoint2{};	// bezier
	int damage{};
	// todo: damage

	xx::Task<> mainTask;
	xx::Task<> MainTask();

	void Init(ItemManagerBase* im_, xx::Weak<Human> owner_, XY const& tarPos_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};



struct Bullet : SceneItem {
	static constexpr int cTypeId{ 3 };

	static constexpr float cScale{ 0.5f };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	static constexpr float cRadius{ 4 };
	static constexpr float cSpeed{ 400.f / gDesign.fps };
	static constexpr float cLifeSpan{ 0.25 };
	static constexpr int cLifeNumFrames{ int(cLifeSpan / gDesign.frameDelay) };

	xx::Weak<SceneItem> owner;
	int damage{};
	// todo: damage

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> moveTask;
	xx::Task<> MoveTask();

	void Init(ItemManagerBase* im_, xx::Weak<Staff> weapon_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};



struct Human : SceneItem {
	static constexpr int cTypeId{ 4 };

	static constexpr float cIdleScaleYFrom{ 0.9f };
	static constexpr float cIdleScaleYTo{ 1.f };
	static constexpr float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	static constexpr XY cAnchor{ 0.5f, 0 };
	static constexpr XY cDrawOffset{ 0, 4.f };
	static constexpr float cRadius{ 5.f };
	static constexpr std::array<float, 5> cFrameIndexRanges{ 0.f, 3.f, 6.f, 9.f, 12.f };
	static constexpr float cFrameInc{ 12.f / gDesign.fps };
	static constexpr float cSpeed{ 60.f / gDesign.fps };

	static constexpr float cAttackRadius{ 300.f };
	static constexpr float cAttackDelaySeconds{ 0.5f };

	float speed{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	void SetDirection(MoveDirections d);
	XY GetPhysPos();

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> idleTask;
	xx::Task<> IdleTask();
	xx::Task<> autoAttackTask;
	xx::Task<> AutoAttackTask();

	void Init(ItemManagerBase* im_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};



struct Monster1 : PhysSceneItem {
	static constexpr int cTypeId{ 5 };

	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 1 };
	static constexpr float cRadius{ cScale * 7 };

	int tracksIndex{};
	float pointIndex{}, speed{};

	virtual bool Hit(SceneItem& attacker, int damage) override;

	void Init(ItemManagerBase* im_, int tracksIndex_, float pointIndex_, float speed_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};




struct SceneTest1 : Scene {
	SpaceGridC<PhysSceneItem, XY> sgcPhysItems;	// must at top
	xx::Shared<Node> rootNode;
	Camera camera;
	ItemManager<Staff, Bomb, Bullet, Human, Monster1> im;
	xx::Weak<Human> human;	// point to im human Item
	bool isBorderVisible{};

	static constexpr int trackCount{ 10 };
	static constexpr int trackDrawStep{ 100 };
	static constexpr float trackStep{ 0.1f };
	static constexpr float trackWidth{ 100 };
	static constexpr float trackMargin{ trackWidth / trackCount };
	static constexpr float trackBaseSpeed{ 5 };
	MovePathCache mpc;
	int trackPointCount{};	// cache: == scene->mpc.points.size()
	xx::Listi32<xx::Listi32<XY>> tracks;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
