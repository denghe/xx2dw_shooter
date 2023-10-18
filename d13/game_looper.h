#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	xx::Shared<RichLabel> rn;
};
extern GameLooper gLooper;
