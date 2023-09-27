#pragma once
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
#include "engine_linestrip.h"

int32_t main();

constexpr GDesign<1280, 720, 1000> gDesign;

struct DragCircle {
	constexpr static float cSpeed{ 1000.f / gDesign.fps };

	void Init(XY const& pos, float radius, int32_t segments);
	bool OnMouseDown();
	void OnMouseMove();
	void Draw();

	XY pos{}, offset{};
	float radius{}, speed{ cSpeed };
	LineStrip border;
};

struct DragCircleShadow {
	constexpr static float cAlphaDecrease{ 1.f / 0.1 / gDesign.fps };

	void Init();
	void Draw();
	xx::Task<> mainTask{ MainTask() };
	xx::Task<> MainTask();

	float alpha{1};
	LineStrip border;
};

struct Poly {
	std::array<XY, 4> vertices;
	LineStrip border;
	void Init();
	void Draw();
};

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;

	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	DragCircle dc;
	xx::ListLink<DragCircleShadow, int32_t> shadows;
	Poly poly;

	DragCircle* mouseFocus{};

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;
