#pragma once
#include <engine.h>
#include "res_frames.h"

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	void ImGuiInit();

	xx::Task<> MainTask();

	// all res pics heree
	ResFrames res;

	Scale9SpriteConfig s9cfg;
};

extern Looper gLooper;
extern ResFrames& gRes;	// == gLooper.frame
// ... more global ref fields here ?
