#pragma once
#ifndef XX2DW_SHOOTER_PCH_H_
#define XX2DW_SHOOTER_PCH_H_

#include <engine.h>

constexpr GDesign<1280, 800, 60> gDesign;
constexpr float gScale = 4;	// for pixel texture
constexpr float gMaxFramePixelWidth = 64;
constexpr float gMaxFramePixelHeight = 128;
constexpr int32_t gGridCellDiameter = 64, gGridNumCols = 10, gGridNumRows = 10;	// tiled map width height + num cols num rows
constexpr Vec2<int32_t> gGridBasePos{ gGridCellDiameter * gGridNumCols / 2, gGridCellDiameter * gGridNumRows / 2 };
// todo: limit shooter move area
constexpr float gSQ = 0.70710678f;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ObjBase {
	XY pos{};
	float radians{};
	float frameIndex{};
	xx::Task<> mainLogic;
	virtual void Draw() = 0;
	virtual ~ObjBase() {}
};

struct MonsterBase : SpaceGridCItem<MonsterBase>, ObjBase {
	float radius{};

	xx::ListDoubleLink<xx::Shared<MonsterBase>, int32_t, uint32_t>* owner{};	// fill before init
	xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t> ivAtOwner;				// fill before init
	void RemoveFromOwner();		// remove from all containers ( maybe call destructor )

	void GridInit();		// call it before: set pos
	void GridUpdate();		// call it before: set pos
	virtual void Hit(int hp) = 0;
	~MonsterBase();
};

struct Shooter;
struct ShooterBullet1;
struct Tree;
struct Explosion;
struct DamageText;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};
	
	// physics containers;
	SpaceGridAB<Tree> sgabTrees;
	SpaceGridC<MonsterBase> sgcMonsters;
	SpaceGridRingDiffuseData sgrdd;


	// tiled map container
	xx::Shared<TMX::Map> tiledMap;
	int32_t tileWidth{}, tileHeight{}, tileWidth_2{}, tileHeight_2{};
	TMX::Layer_Tile* layerBG{}, *layerTrees{};
	Camera camera;

	// res
	xx::Shared<Frame> frame_shooter, frame_tree;
	std::vector<xx::Shared<Frame>>
		frames_monster_1, frames_monster_2, frames_monster_3
		, frames_explosion, frames_bullets;
	// ...

	// effects
	xx::ListLink<xx::Shared<Explosion>, int32_t> effects_explosion;
	xx::ListLink<xx::Shared<DamageText>, int32_t> effects_damageText;

	// objs
	xx::Shared<Shooter> shooter;
	xx::ListLink<xx::Shared<ShooterBullet1>, int32_t> bullets_shooter1;
	std::vector<xx::Shared<Tree>> trees;
	xx::ListDoubleLink<xx::Shared<MonsterBase>, int32_t, uint32_t> monsters;

	std::vector<std::pair<float, ObjBase*>> tmpsPosYObj;	// for sort by Y

	template<typename MT>
	xx::Shared<MT>& CreateMonster() {
		auto&& m = monsters.Emplace().Emplace<MT>();
		m->owner = &monsters;
		m->ivAtOwner = monsters.Tail();
		return m;
	}
	MonsterBase* FindNeighborMonster(XY const& pos, float radius);
	MonsterBase* FindNearestMonster(XY const& pos, float maxDistance);
};
extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct DamageText : ObjBase {
	constexpr static int32_t cLife{ 30 };
	constexpr static float moveSpeed{ 1 };

	RGBA8 color{};
	std::string txt;

	void Init(XY const& bornPos, int32_t hp, RGBA8 color_ = { 255, 255, 255, 255 });
	void Draw() override;
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Explosion : ObjBase {
	constexpr static float cScale{ gScale / 2 };
	constexpr static float cFrameMaxIndex{ 5.f };
	constexpr static float cFrameInc{ 0.1f };

	Quad body;

	void Init(XY const& bornPos);
	void Draw() override;
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Tree : ObjBase, SpaceGridABItem<Tree> {
	constexpr static int cXOffset{ 32 }, cYOffset{ 24 };
	constexpr static int cBoxWidth{ 36 }, cBoxHeight{ 16 };

	Quad body;

	void Init(float x, float y);
	void Draw() override;
	~Tree();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter : ObjBase {
	constexpr static float cRadius{ 8 * gScale };
	constexpr static float cSpeed{ 40 * gScale / gDesign.fps };
	constexpr static float cFrameMaxChangeRadian{ float(M_PI * 10 / gDesign.fps) };
	constexpr static float cFireDistance{ cRadius };
	constexpr static float cTouchDistance{ 40 };

	Quad body;
	float touchLastRotation{};

	void Init(XY const& bornPos);
	void Draw() override;
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ShooterBullet1 : ObjBase {
	constexpr static int cFrameIndex{ 0 };
	constexpr static int cLife{ 5 * (int)gDesign.fps };
	constexpr static float cRadius{ 2 * gScale };
	constexpr static float cSpeed{ 60 * gScale / gDesign.fps };
	constexpr static int cDamage{ 5 }, cMaxDamage{ 20 };

	Quad body;
	XY inc{};
	int damage{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	void Draw() override;
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Monster1 : MonsterBase {
	constexpr static float cScale{ gScale / 2 }, cMaxScale{ gScale * 2 };		// limit by sgc cell size
	constexpr static float cScaleSteps{ gDesign.fps };
	constexpr static float cRadius{ 16 };	// map to cScale
	constexpr static float cSpeed{ 20 * gScale / gDesign.fps };
	constexpr static float cFrameMaxIndex{ 6.f };
	constexpr static float cFrameInc{ 0.1f * 60 / gDesign.fps };
	constexpr static int cLife{ 30 * (int)gDesign.fps };
	constexpr static int cHP{ 100 };
	constexpr static int cHertLife{ int(10 * gDesign.fps / 60) };

	Quad body;
	float frameIndex{}, scale{};
	int life{ cLife }, hp{ cHP }, hertLife{};

	void Init(XY const& bornPos);
	void Hit(int damage) override;
	void Draw() override;
	xx::Task<> MainLogic();
	xx::TaskGuard hitLogic;
};

#endif
