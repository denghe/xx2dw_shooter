﻿#pragma once
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

constexpr GDesign<1280, 720, 120> gDesign;

struct DragCircle {
	constexpr static float cSpeed{ 1000.f / gDesign.fps };
	constexpr static RGBA8 cColor{ 255, 255, 0, 255 };

	void Init(XY const& pos, float radius, int32_t segments);
	bool OnMouseDown();
	void OnMouseMove();
	void Draw();

	XY pos{}, offset{};
	float radius{}, speed{ cSpeed };
	RGBA8 color{ cColor };
	LineStrip border;
};

struct Poly {
	constexpr static float cRadiansIncrease{ M_PI * 2 * 2 / gDesign.fps };

	void Init();
	void Draw();
	xx::Task<> mainTask{ MainTask() };
	xx::Task<> MainTask();

	std::array<XY, 5> vertsForDraw, vertsForCalc, vertsForCalcBak;
	LineStrip border;
	XY pos{}, scale{1,1};
	float radians{};
};

struct Shadow {
	constexpr static float cAlphaDecrease{ 1.f / 0.1 / gDesign.fps };

	void Init(LineStrip const& border_);
	void Draw();
	xx::Task<> mainTask{ MainTask() };
	xx::Task<> MainTask();

	float alpha{ 1 };
	LineStrip border;
};



struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;
	constexpr static int shadowsCap = fps * 2;
	std::string log;

	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	DragCircle dc;
	xx::ListLink<Shadow, int32_t, shadowsCap> shadows;
	Poly poly;

	DragCircle* mouseFocus{};

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;