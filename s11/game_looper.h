#pragma once
#include "engine_engine.h"

int32_t main();

constexpr static GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	LineStrip ls;
};
extern GameLooper gLooper;
