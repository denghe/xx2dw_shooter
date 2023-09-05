#pragma once
#ifndef XX2DW_SHOOTER_PCH_H_
#define XX2DW_SHOOTER_PCH_H_

#include "engine_engine.h"
#include "engine_gdesign.h"
#include "engine_chartexcache.h"
#include "engine_fpsviewer.h"
#include "engine_dynamictexturepacker.h"
#include "engine_curvemovepath.h"
#include "engine_spacegrid_box.h"
#include "engine_spacegrid_circle.h"
#include "engine_spacegrid_ringdiffuse.h"

int32_t main();

constexpr GDesign<1280, 800, 256> gDesign;
constexpr float gScale = 2;	// scale texture

struct ObjBase : Quad, xx::Tasks {
	float frameIndex{};
	bool disposing{};
};

struct Shooter;
struct ShooterBullet1;

enum class KeyboardKeys {
	A = 65,
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
	, MAX_VALUE
};

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;

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

	// flags
	bool ready{};
	
	// res
	xx::Shared<Frame> frame_shooter;
	std::vector<xx::Shared<Frame>> frames_bullets;
	// ...

	// player objs
	xx::Shared<Shooter> shooter;
	xx::ListLink<xx::Shared<ShooterBullet1>, int32_t> bullets_shooter1;
};
extern GameLooper gLooper;

struct Shooter : ObjBase {
	constexpr static float cRadius{ 8 * gScale }, cSpeed{ 180 / gDesign.fps }, cFrameMaxChangeRadian{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDistance{ cRadius };
	constexpr static float cTouchDistance{ 40 };

	float touchLastRotation{};

	void Init();
	xx::Task<> MainLogic();
	std::optional<XY> GetKeyboardMoveInc();
};

struct ShooterBullet1 : ObjBase {
	constexpr static int cFrameIndex{ 0 };
	constexpr static float cRadius{ 8.f };
	constexpr static float cSpeed{ 2 * 60 / gDesign.fps };

	XY inc{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	xx::Task<> MainLogic();
};

#endif
