#pragma once
#include "pch.h"
#include "res_frames.h"

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();

	// res
	ResFrames res;

	// cfgs
	Scale9SpriteConfig s9cfg_btn;

	// for space grid  FindNearest  ForeachByRange
	xx::SpaceRingDiffuseData sgrdd;
};

extern Looper gLooper;
extern ResFrames& gRes;
