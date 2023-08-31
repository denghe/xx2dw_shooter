#include "pch.h"

int main();


/*****************************************************************************************************/
/*****************************************************************************************************/

enum class ObjTypes : int {
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

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter;
struct Monster1;
struct Monster2;
struct Monster3;
struct Explosion;
struct DamageText;
struct ShooterBullet1;

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
	std::array<bool, (int)KeyboardKeys::MAX_VALUE> keyboardKeysStates{};
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

	// res
	xx::Shared<Frame> frame_shooter;
	std::vector<xx::Shared<Frame>> 
		frames_monster_1
		, frames_monster_2
		, frames_monster_3
		, frames_explosion
		;
	// ...

	// player objs
	xx::Shared<Shooter> shooter;
	xx::ListLink<xx::Shared<ShooterBullet1>, int> bullets_shooter1;

	// monster objs
	xx::ListLink<xx::Shared<ObjBase>, int> monsters;
	//xx::ListLink<xx::Shared<XXXXXXXXXXX>, int> bullets_monster1;

	// effects
	xx::ListLink<xx::Shared<Explosion>, int> effects_explosion;
	xx::ListLink<xx::Shared<DamageText>, int> effects_damageText;
	// ...
};

extern GameLooper gLooper;
constexpr GDesign<1024, 768> gDesign;
constexpr float gScale = 4;
static constexpr const float gSQ = 0.7071067811865475244;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Shooter : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Shooter };
	constexpr static float cRadius{ 32 }, cSpeed{ 2 };
	constexpr static float cBulletRadius{ 8 }, cBulletSpeed{ 4 };
	constexpr static float cFireDistance{ 30 };
	constexpr static float cTouchDistance{ 40 };

	float touchLastRotation{};

	void Init();
	void Draw();
	xx::Task<> MainLogic();
	std::optional<XY> GetKeyboardMoveInc();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ShooterBullet1 : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::ShooterBullet1 };
	constexpr static float cRadius{ 16 };

	XY inc{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct DamageText : ObjBase {
	constexpr static int cLife{ 30 };

	std::string txt;

	void Init(XY const& bornPos, int hp);
	void Draw();
	xx::Task<> MainLogic();
};

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Monster1 : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster1 };
	constexpr static float cRadius{ 10.f };
	constexpr static float cFrameMaxIndex{ 6.f };
	constexpr static float cFrameInc{ 0.1f };
	constexpr static int cLife{ 60 * 3 };

	int life{ cLife };
	float frameIndex{};

	void Init();
	void Draw();
	xx::Task<> MainLogic();
};


/*****************************************************************************************************/
/*****************************************************************************************************/
// todo

struct Monster2 : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster2 };
	constexpr static float cRadius{ 10.f };
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Monster3 : ObjBase {
	constexpr static ObjTypes cType{ ObjTypes::Monster3 };
	constexpr static float cRadius{ 10.f };
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Explosion : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};
