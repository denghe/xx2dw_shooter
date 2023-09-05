#include "pch.h"

int32_t main();


/*****************************************************************************************************/
/*****************************************************************************************************/

constexpr GDesign<1024, 768> gDesign;
constexpr float gScale = 4;	// scale texture
constexpr int32_t gGridCellDiameter = 16, gGridNumCols = 256, gGridNumRows = 256;
constexpr Vec2<int32_t> gGridBasePos{ gGridCellDiameter * gGridNumCols / 2, gGridCellDiameter * gGridNumRows / 2 };
constexpr float gSQ = 0.7071067811865475244;

struct ObjBase : Quad, xx::Tasks {
	float frameIndex{};
	bool disposing{};
};

enum class MonsterTypes : int32_t {
	Unknown,
	Monster1,
	Monster2,
	Monster3,
	// ...
};

struct MonsterBase : SpaceGridCItem<MonsterBase>, ObjBase {
	MonsterTypes type{};
	float radius{};

	xx::ListDoubleLink<xx::Shared<MonsterBase>, int32_t, uint32_t>* owner{};	// fill before init
	xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t> ivAtOwner;				// fill before init
	void RemoveFromOwner();		// remove from all containers

	void GridInit();		// call it before: set pos
	void GridUpdate();		// call it before: set pos
	~MonsterBase();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter;
struct ShooterBullet1;
struct ShooterBullet2;
struct Explosion;
struct DamageText;

enum class KeyboardKeys {
	A = 65,
	B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z
	, MAX_VALUE
};

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;

    CharTexCache<24> ctc24;
    CharTexCache<72> ctc72;
    FpsViewer fv;

    XY mousePos;
	std::array<bool, 16> mouseBtnStates{};
	std::array<bool, (int32_t)KeyboardKeys::MAX_VALUE> keyboardKeysStates{};
	long aimTouchId{ -1 }, fireTouchId{ -1 };
	XY aimTouchStartPos, aimTouchMovePos;	// virtual joy
	bool touchMode{};

    EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e);
    EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e);

    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	EM_BOOL OnTouchStart(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchMove(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchEnd(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchCancel(EmscriptenTouchEvent const& e);


	bool Pressed(KeyboardKeys k) const;

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// physics containers ( Place on top of business objects )
	SpaceGridC<MonsterBase> sgc;
	SpaceGridRingDiffuseData<gGridNumRows, gGridCellDiameter> sgrdd;

	// res
	xx::Shared<Frame> frame_shooter;
	std::vector<xx::Shared<Frame>> 
		frames_monster_1
		, frames_monster_2
		, frames_monster_3
		, frames_explosion
		, frames_bullets
		;
	// ...

	// player objs
	xx::Shared<Shooter> shooter;
	xx::ListLink<xx::Shared<ShooterBullet1>, int32_t> bullets_shooter1;
	xx::ListLink<xx::Shared<ShooterBullet2>, int32_t> bullets_shooter2;

	// monster objs
	xx::ListDoubleLink<xx::Shared<MonsterBase>, int32_t, uint32_t> monsters;
	//xx::ListLink<xx::Shared<XXXXXXXXXXX>, int32_t> bullets_monster1;

	template<typename MT>
	xx::Shared<MT>& NewMonster(XY const& bornPos) {
		auto&& m = monsters.Emplace().Emplace<MT>();
		m->owner = &monsters;
		m->ivAtOwner = monsters.Tail();
		m->Init(bornPos);
		return m;
	}

	MonsterBase* FindNeighborMonster(XY const& pos, float radius) {
		auto p = gGridBasePos.MakeAdd(pos);
		auto crIdx = sgc.PosToCrIdx(p);
		MonsterBase* r{};
		sgc.Foreach9(crIdx, [&](MonsterBase* m)->bool {
			// (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
			auto d = m->pos - pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (rr > dd) {
				r = m;
				return true;
			}
			return false;
		});
		return r;
	}

	MonsterBase* FindNearestMonster(XY const& pos, float maxDistance) {
		auto p = gGridBasePos.MakeAdd(pos);						// convert pos to grid coordinate
		auto crIdx = sgc.PosToCrIdx(p);							// calc grid col row index

		float minVxxyy = maxDistance * maxDistance;
		MonsterBase* o{};
		XY ov;

		auto& lens = sgrdd.lens;
		auto& idxs = sgrdd.idxs;
		for (int i = 1; i < lens.len; i++) {
			if (lens[i].radius > maxDistance) break;			// limit search range

			auto offsets = &idxs[lens[i - 1].count];
			auto size = lens[i].count - lens[i - 1].count;
			sgc.ForeachCells(crIdx, offsets, size, [&](MonsterBase* m)->bool {
				auto v = m->pos - pos;
				if (auto xxyy = v.x * v.x + v.y * v.y; xxyy < minVxxyy) {
					minVxxyy = xxyy;
					o = m;
					ov = v;
				}
				return false;
				});

			if (o) return o;									// found. stop ring diffuse step
		}
		return nullptr;
	}

	// effects
	xx::ListLink<xx::Shared<Explosion>, int32_t> effects_explosion;
	xx::ListLink<xx::Shared<DamageText>, int32_t> effects_damageText;
	// ...
};

extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter : ObjBase {
	constexpr static float cRadius{ 32 }, cSpeed{ 5 };
	constexpr static float cFireDistance{ 30 };
	constexpr static float cTouchDistance{ 40 };

	float touchLastRotation{};

	void Init();
	xx::Task<> MainLogic();
	std::optional<XY> GetKeyboardMoveInc();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ShooterBullet1 : ObjBase {
	constexpr static int cFrameIndex{ 0 };
	constexpr static float cRadius{ 8.f };
	constexpr static float cSpeed{ 4 };

	XY inc{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ShooterBullet2 : ObjBase {
	constexpr static int cFrameIndex{ 4 };
	constexpr static float cRadius{ 8.f }, cSpeed{ 3 }, cMaxLookupDistance{ 100 };

	XY inc{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct DamageText : ObjBase {
	constexpr static int32_t cLife{ 30 };

	std::string txt;

	void Init(XY const& bornPos, int32_t hp);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Explosion : ObjBase {
	constexpr static float cFrameMaxIndex{ 5.f };
	constexpr static float cFrameInc{ 0.1f };

	void Init(XY const& bornPos);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

// green
struct Monster1 : MonsterBase {
	constexpr static MonsterTypes cType{ MonsterTypes::Monster1 };
	constexpr static float cScale{ 0.5f },cRadius{ 7.f }, cSpeed{ 2 };
	constexpr static float cFrameMaxIndex{ 6.f };
	constexpr static float cFrameInc{ 0.1f };
	constexpr static int32_t cLife{ 60 * 20 };

	int32_t life{ cLife };
	float frameIndex{};

	void Init(XY const& bornPos);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

// orange
struct Monster2 : MonsterBase {
	constexpr static MonsterTypes cType{ MonsterTypes::Monster2 };
	constexpr static float cRadius{ 7.f };
	constexpr static float cFrameMaxIndex{ 5.f };
	constexpr static float cFrameInc{ 0.1f };
	constexpr static int32_t cLife{ 60 * 60 };

	int32_t life{ cLife };
	float frameIndex{};

	void Init(XY const& bornPos);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

// pink
struct Monster3 : MonsterBase {
	constexpr static MonsterTypes cType{ MonsterTypes::Monster3 };
	constexpr static float cRadius{ 7.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 0.1f };
	constexpr static int32_t cLife{ 60 * 60 };

	int32_t life{ cLife };
	float frameIndex{};

	void Init(XY const& bornPos);
	void Draw();
	xx::Task<> MainLogic();
};
