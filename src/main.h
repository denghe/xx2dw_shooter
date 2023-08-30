#include "pch.h"

int main();

struct Shooter;
struct Monster1;
struct Monster2;
struct Monster3;
struct Explosion;
struct DamageText;
struct ShooterBullet1;

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = 60, frameDelay = 1.f / fps, maxFrameDelay = frameDelay * 3;

    CharTexCache<24,32,32,2048,2048> ctc24;
    FpsViewer fv;
    XY mousePos;
	std::array<bool, 16> mouseBtnStates{};
    EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
    EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	// todo: keyboard event

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

struct gDesign_ {
	static constexpr float width = 1280;
	static constexpr float height = 800;
	static constexpr float width_2 = width / 2;
	static constexpr float height_2 = height / 2;
	/*
		screen design anchor point
	   ┌───────────────────────────────┐
	   │ 7             8             9 │
	   │                               │
	   │                               │
	   │ 4             5             6 │
	   │                               │
	   │                               │
	   │ 1             2             3 │
	   └───────────────────────────────┘
	*/
	static constexpr float x1 = -width_2;
	static constexpr float y1 = -height_2;
	static constexpr float x2 = 0.f;
	static constexpr float y2 = -height_2;
	static constexpr float x3 = width_2;
	static constexpr float y3 = -height_2;
	static constexpr float x4 = -width_2;
	static constexpr float y4 = 0.f;
	static constexpr float x5 = 0.f;
	static constexpr float y5 = 0.f;
	static constexpr float x6 = width_2;
	static constexpr float y6 = 0.f;
	static constexpr float x7 = -width_2;
	static constexpr float y7 = height_2;
	static constexpr float x8 = 0.f;
	static constexpr float y8 = height_2;
	static constexpr float x9 = width_2;
	static constexpr float y9 = height_2;
} constexpr gDesign;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct ObjBase : Quad, xx::Tasks {
	float frameIndex{};
	bool disposing{};
};

struct Monster1 : ObjBase {
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

struct DamageText : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct Shooter : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};

struct ShooterBullet1 : ObjBase {
	void Init();
	void Draw();
	xx::Task<> MainLogic();
};
