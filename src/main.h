#include "pch.h"

int main();

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

    CharTexCache<24,32,32,2048,2048> ctc24;
    FpsViewer fv;

    XY mousePos;
	std::array<bool, 16> mouseBtnStates{};
	std::array<bool, (int)KeyboardKeys::MAX_VALUE> keyboardKeysStates{};

    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

    EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e);
    EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e);

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

	// objs
	xx::Shared<Shooter> shooter;

	xx::ListLink<xx::Shared<Monster1>, int> monsters_1;
	xx::ListLink<xx::Shared<Monster2>, int> monsters_2;
	xx::ListLink<xx::Shared<Monster3>, int> monsters_3;
	// ...

	xx::ListLink<xx::Shared<Explosion>, int> effects_explosion;
	xx::ListLink<xx::Shared<DamageText>, int> effects_damageText;
	// ...

	xx::ListLink<xx::Shared<ShooterBullet1>, int> bullets_shooter1;
	// ...
};

extern GameLooper gLooper;
constexpr GDesign<1280, 800> gDesign;
constexpr float gScale = 4;
static constexpr const float gSQ = 0.7071067811865475244;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ObjBase : Quad, xx::Tasks {
	float frameIndex{};
	bool disposing{};
};


struct Shooter : ObjBase {
	constexpr static float radius{ 32 }, diameter = radius * 2, speed{ 2 };
	constexpr static float bulletRadius{ 8 }, bulletSpeed{ 4 };
	constexpr static float fireDistance{ 30 };
	void Init();
	void Draw();
	xx::Task<> MainLogic();
	std::pair<bool, XY> GetASDWMoveInc();
};


struct ShooterBullet1 : ObjBase {
	constexpr static float radius{ 16 }, diameter = radius * 2;
	XY inc{};
	Quad body;
	void Init(XY const& pos_, XY const& inc_, float radians_);
	void Draw();
	xx::Task<> MainLogic();
};


struct DamageText : ObjBase {
	constexpr static int defaultLife{ 30 };
	int hp{}, life{ defaultLife };
	XY inc;
	std::string txt;
	void Init(XY const& pos_, int hp_);
	void Draw();
	xx::Task<> MainLogic();
};


struct Monster1 : ObjBase {
	constexpr static float frameMaxIndex{ 6.f };
	constexpr static float frameInc{ 0.1f };
	constexpr static int defaultLife{ 60 * 3 };
	int life{ defaultLife };
	float frameIndex{};
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Monster2 : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Monster3 : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Explosion : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};
