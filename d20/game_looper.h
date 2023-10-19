#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	xx::Task<> MainTask();

	// res
	xx::List<xx::Ref<Frame>, int32_t> frames_gem;
	xx::Ref<Frame> frame_button;
	// ...
};

extern GameLooper gLooper;
