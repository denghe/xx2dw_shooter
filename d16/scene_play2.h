#pragma once
#include <game_looper.h>
#include <game_item.h>

struct MapCfg {
	static constexpr int physCellSize{ 32 };	// need >= max monster size
	static constexpr int physNumRows{ 128 };
	static constexpr int physNumCols{ 128 };
	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapCenterPos{ mapSize.x / 2, mapSize.y / 2 };
};
inline MapCfg gMapCfg;



struct ScenePlay2;

struct SceneItem : Item {
	ScenePlay2* scene{};			// Init(ItemManagerBase* im_, ...) { ... scene = (ScenePlay2*)im_->userData;
	void SceneItemInit(int typeId_, ItemManagerBase* im_);
};

struct ScenePhysItem : SceneItem, SpaceGridCItem<ScenePhysItem> {
	void PhysAdd();				// call after fill pos
	void PhysUpdate();			// call after fill pos
	void PhysRemove();
	~ScenePhysItem();
};



struct BornMask {
	static constexpr float cVisibleTimeSpan{ 0.2f };
	static constexpr float cHideTimeSpan{ 0.1f };

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

// todo: blood effect, hp bar, number effect, shadow ...
// exp? level? hp? damage?


struct Human;
struct Weapon : SceneItem {
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
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};


// todo: bullet tail ?

struct Bullet : SceneItem {
	static constexpr int cTypeId{ 2 };

	static constexpr float cFrameMaxIndex = 8.f;
	static constexpr float cFrameInc{ 12.f / gDesign.fps };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	static constexpr float cRadius{ 4 };
	static constexpr float cSpeed{ 60 };
	static constexpr float cSpeedByFrame{ cSpeed / gDesign.fps };
	static constexpr float cLifeSpan{ 2 };
	static constexpr int cLifeNumFrames{ int(cLifeSpan / gDesign.frameDelay) };

	xx::Weak<SceneItem> owner;
	// todo: damage

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> moveTask;
	xx::Task<> MoveTask();

	void Init(ItemManagerBase* im_, xx::Weak<Weapon> weapon_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};



struct Human : SceneItem {
	static constexpr int cTypeId{ 3 };

	static constexpr float cIdleScaleYFrom{ 0.9f };
	static constexpr float cIdleScaleYTo{ 1.f };
	static constexpr float cIdleScaleYStep{ (cIdleScaleYTo - cIdleScaleYFrom) * 2 / gDesign.fps };

	static constexpr XY cAnchor{ 0.5f, 0 };
	static constexpr XY cDrawOffset{ 0, 4.f };
	static constexpr float cRadius{ 5.f };
	static constexpr std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	static constexpr float cFrameInc{ 12.f / gDesign.fps };
	static constexpr float cSpeed{ 60.f / gDesign.fps };

	float speed{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	void SetDirection(MoveDirections d);
	XY GetPhysPos();

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> idleTask;
	xx::Task<> IdleTask();

	void Init(ItemManagerBase* im_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};



struct Slime : ScenePhysItem {
	static constexpr int cTypeId{ 4 };

	static constexpr XY cBornMaskAnchor{ 0.5f, 0.25f };
	static constexpr float cBornMaskScale{ 1.f };
	static constexpr float cBornIdleDelay{ 0.5f };
	static constexpr int cBornIdleDelayFrames{ int(cBornIdleDelay / gDesign.frameDelay) };

	static constexpr XY cAnchor{ 0.5f, 0 };
	static constexpr XY cDrawOffset{ 0, 6.f };
	static constexpr float cRadius{ 6.f };
	static constexpr float cFrameMaxIndex = 4.f;
	static constexpr float cFrameInc{ 12.f / gDesign.fps };
	static constexpr float cSpeed{ 30.f / gDesign.fps };
	static constexpr float cStepMoveDuration{ 0.5f };

	static constexpr float cLifeSpan{ 10 };
	static constexpr int cLifeNumFrames{ int(cLifeSpan / gDesign.frameDelay) };

	float speed{};
	bool freeze{};		// fozen, hert, ...
	// todo: hp? exp?

	xx::Task<> mainTask;
	xx::Task<> MainTask();
	xx::Task<> animTask;
	xx::Task<> AnimTask();
	xx::Task<> moveTask;
	xx::Task<> MoveTask();
	void Init(ItemManagerBase* im_, XY const& pos_);
	virtual int UpdateCore() override;
	virtual void Draw(Camera const& camera) override;
};


struct ScenePlay2 : Scene {
	xx::Shared<Node> rootNode;

	SpaceGridC<ScenePhysItem> sgcPhysItems;	// must at top
	BornMaskManager bmm;
	ItemManager<Weapon, Bullet, Human, Slime> im;
	xx::Weak<Human> human;	// point to im human Item
	Camera camera;
	xx::Listi32<ItemY> iys;
	bool isBorderVisible{};
	// ...


	void MakeSlime();	// random position

	// ... more create funcs

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
