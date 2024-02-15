#pragma once
#include <game_looper.h>
#include <game_item.h>

struct MapCfg {
	constexpr static int physCellSize{ 32 };	// need >= max monster size
	constexpr static int physNumRows{ 128 };
	constexpr static int physNumCols{ 128 };
	constexpr static XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	constexpr static XY mapCenterPos{ mapSize.x / 2, mapSize.y / 2 };
};
inline MapCfg gMapCfg;



struct ScenePlay2;

struct SceneItem : Item {
	ScenePlay2* scene{};			// Init(ItemManagerBase* im_, ...) { ... scene = (ScenePlay2*)im_->userData;
	void SceneItemInit(int typeId_, ItemManagerBase* im_);
};

struct ScenePhysItem : SceneItem, SpaceGridCItem<ScenePhysItem> {
	XY physOffset{};			// anchor point pos fix. need set value at init
	XY GetPhysPos();
	void PhysAdd();				// call after fill pos
	void PhysUpdate();			// call after fill pos
	void PhysRemove();
	~ScenePhysItem();
};


struct BornMask {
	constexpr static float cVisibleTimeSpan{ 0.2f };
	constexpr static float cHideTimeSpan{ 0.1f };

	XY pos{}, anchor{};
	float scale{};
	bool visible{};
	int lineNumber{}, i{}, e{};
	std::function<void()> onDispose;
	void Init(std::function<void()> onDispose_, XY const& pos_, XY const& anchor_, float scale_);
	int UpdateCore();
	bool Update();
	void Draw(Camera const& camera);
};

struct BornMaskManager {
	xx::ListLink<BornMask> os;
	void Init(size_t cap);
	void Add(std::function<void()> onDispose_, XY const& pos_, XY const& anchor_, float scale_);
	bool Update();
	void Draw(Camera const& camera);
};

struct Human;
struct Weapon {
	xx::Weak<Human> owner;
	// todo
};

struct Bullet {
	xx::Weak<Human> owner;
	// todo
};

struct Human : SceneItem {
	static constexpr int cTypeId{ 1 };

	constexpr static float cIdleScaleYFrom{ 0.9f };
	constexpr static float cIdleScaleYTo{ 1.f };
	constexpr static float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	constexpr static XY cAnchor{ 0.5f, 0 };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 60.f / gDesign.fps };

	float speed{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	void SetDirection(MoveDirections d);

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> idleTask;
	xx::Task<> IdleTask();

	void Init(ItemManagerBase* im_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};



struct Slime : ScenePhysItem {
	static constexpr int cTypeId{ 2 };

	constexpr static XY cBornMashAnchor{ 0.5f, 0.25 };
	constexpr static float cBornMashScale{ 1.f };
	constexpr static XY cAnchor{ 0.5f, 0 };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	static constexpr float cLifeSpan{ 10 };
	static constexpr int cLifeNumFrames{ int(cLifeSpan / gDesign.frameDelay) };

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	void Init(ItemManagerBase* im_, XY const& pos_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};


struct ScenePlay2 : Scene {
	xx::Shared<Node> rootNode;

	SpaceGridC<ScenePhysItem> sgcPhysItems;	// must at top
	BornMaskManager bmm;
	ItemManager<Human, Slime> im;
	xx::Weak<Human> human;	// point to im human Item
	Camera camera;
	xx::Listi32<ItemY> iys;
	bool physBorderVisible{};
	// ...


	void MakeSlime();	// random position

	// ... more create funcs

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
