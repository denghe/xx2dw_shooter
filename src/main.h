#include "pch.h"

int32_t main();


/*****************************************************************************************************/
/*****************************************************************************************************/

enum class ObjTypes : int32_t {
	Unknown,
	Shooter,
	ShooterBullet1,
	Monster1,
	Monster2,
	Monster3,
	// ...
};

struct ObjBase : Quad, xx::Tasks {
	ObjTypes type{};
	float radius{};
	float frameIndex{};
	bool disposing{};
};

struct GridObjBase : SpaceGridCItem<GridObjBase>, ObjBase {

	xx::ListDoubleLink<xx::Shared<GridObjBase>, int32_t, uint32_t>* owner{};	// fill before init
	xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t> ivAtOwner;				// fill before init
	void RemoveFromOwner();		// remove from all containers

	void GridInit();		// call it before: set pos
	void GridUpdate();		// call it before: set pos
	~GridObjBase();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter;
struct ShooterBullet1;
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
	SpaceGridC<GridObjBase> sgc;

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

	// monster objs
	xx::ListDoubleLink<xx::Shared<GridObjBase>, int32_t, uint32_t> monsters;
	//xx::ListLink<xx::Shared<XXXXXXXXXXX>, int32_t> bullets_monster1;

	template<typename MT>
	xx::Shared<MT>& NewMonster(XY const& bornPos) {
		auto&& m = monsters.Emplace().Emplace<MT>();
		m->owner = &monsters;
		m->ivAtOwner = monsters.Tail();
		m->Init(bornPos);
		return m;
	}

	// effects
	xx::ListLink<xx::Shared<Explosion>, int32_t> effects_explosion;
	xx::ListLink<xx::Shared<DamageText>, int32_t> effects_damageText;
	// ...
};

extern GameLooper gLooper;
constexpr GDesign<1024, 768> gDesign;
constexpr float gScale = 4;	// scale texture
constexpr int32_t gGridDiameter = 64, gGridRadius = gGridDiameter / 2, gGridWidth = 512, gGridHeight = 512;
constexpr Vec2<int32_t> gGridBasePos{ gGridDiameter * gGridWidth / 2, gGridDiameter * gGridHeight / 2};
constexpr float gSQ = 0.7071067811865475244;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Shooter };
	constexpr static float cRadius{ 32 }, cSpeed{ 2 };
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
	constexpr static ObjTypes cType{ ObjTypes::ShooterBullet1 };
	constexpr static float cRadius{ 8.f };
	constexpr static float cSpeed{ 4 };

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

struct Monster1 : GridObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster1 };
	constexpr static float cRadius{ 7.f };
	constexpr static float cFrameMaxIndex{ 6.f };
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
// todo

struct Monster2 : GridObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster2 };
	constexpr static float cRadius{ 10.f };
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Monster3 : GridObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster3 };
	constexpr static float cRadius{ 10.f };
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};
