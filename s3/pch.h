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
#include "engine_tiledmap_sede.h"
#include "engine_camera.h"

int32_t main();

constexpr GDesign<1280, 800, 60> gDesign;
constexpr float gScale = 4;	// for pixel texture
constexpr float gMaxFramePixelWidth = 64;
constexpr float gMaxFramePixelHeight = 128;

struct ObjBase : xx::Tasks {
	XY pos{};
	float radians{};
	float frameIndex{};
	bool disposing{};
};

struct Shooter;
struct ShooterBullet1;

// type same as EmscriptenKeyboardEvent.what
using KeyboardKeys_t = decltype(EmscriptenKeyboardEvent::which);
enum class KeyboardKeys : KeyboardKeys_t {
	Unknown = 0,
	A = 65,
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
	, MAX_VALUE
};

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;
	CharTexCache<72> ctc72;
	FpsViewer fv;

	std::array<bool, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysStates{};
	std::array<double, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysDelays{};
	EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e);
	EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e);
	bool KeyDown(KeyboardKeys k) const;
	bool KeyDownDelay(KeyboardKeys k, double delaySecs);

	XY mousePos;
	std::array<bool, 16> mouseBtnStates{};
	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	long aimTouchId{ -1 }, fireTouchId{ -1 };
	XY aimTouchStartPos, aimTouchMovePos;	// virtual joy
	bool touchMode{};
	EM_BOOL OnTouchStart(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchMove(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchEnd(EmscriptenTouchEvent const& e);
	EM_BOOL OnTouchCancel(EmscriptenTouchEvent const& e);

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};
	
	// tiled map container
	xx::Shared<TMX::Map> tiledMap;
	TMX::Layer_Tile* layerBG{}, *layerTrees{};
	Camera camera;

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
	constexpr static float cRadius{ 8 * gScale };
	constexpr static float cSpeed{ 40 * gScale / gDesign.fps };
	constexpr static float cFrameMaxChangeRadian{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDistance{ cRadius };
	constexpr static float cTouchDistance{ 40 };

	Quad body;
	float touchLastRotation{};

	void Init(XY const& bornPos);
	void Draw();
	xx::Task<> MainLogic();
	std::optional<XY> GetKeyboardMoveInc();
};

struct ShooterBullet1 : ObjBase {
	constexpr static int cFrameIndex{ 0 };
	constexpr static int cLife{ 5 * gDesign.fps };
	constexpr static float cRadius{ 2 * gScale };
	constexpr static float cSpeed{ 60 * gScale / gDesign.fps };

	Quad body;
	XY inc{};

	void Init(XY const& bornPos, XY const& inc_, float radians_);
	void Draw();
	xx::Task<> MainLogic();
};

#endif
