#pragma once
#include <engine.h>
#include <res_frames.h>

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();

	// all res pics heree
	ResFrames res;

	Scale9SpriteConfig s9cfg;

	// tiled map & frames
	xx::List<xx::Ref<TMX::Map>> map_stages;

	// for grid  FindNearest  ForeachByRange
	SpaceGridRingDiffuseData sgrdd;


	xx::Ref<TMX::Map> map1, map2;	// todo: remove these code
};

extern Looper gLooper;
extern ResFrames& gRes;	// == gLooper.frame
// ... more global ref fields here ?
