#pragma once
#include <engine.h>
#include <res_frames.h>

constexpr GDesign<1280, 800, 360> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();

	ResFrames res;

	Scale9SpriteConfig s9cfg;
};

extern Looper gLooper;
extern ResFrames& gRes;
