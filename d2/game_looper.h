#pragma once
#ifdef __EMSCRIPTEN__
#include "engine_engine.h"
#else
#include "engine_engine_desktop.h"
#endif

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	xx::Shared<RepeatQuad> rq;
};
extern GameLooper gLooper;
