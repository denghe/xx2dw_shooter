#pragma once
#include "engine_engine.h"

int32_t main();

static constexpr GDesign<1280, 720, 120> gDesign;

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



struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	constexpr static int shadowsCap = fps * 2;
	std::string log;

	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	DragCircle dc;
	xx::ListLink<Shadow, int32_t, shadowsCap> shadows;
	Poly poly;

	DragCircle* mouseFocus{};

	void Update();
	xx::Task<> MainTask();
	void Draw();
};
extern GameLooper gLooper;
