#pragma once
#include "engine_engine_desktop.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	xx::Shared<Frame> frame_button;
};
extern GameLooper gLooper;
