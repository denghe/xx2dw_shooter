#pragma once
#include <engine.h>

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();

	xx::Ref<Frame> frame_td_ui_border
		, frame_td_shape_dot
		, frame_td_shape_ring
		, frame_td_shape_rect
		, frame_td_shape_trangle
		, frame_td_shape_circle;

	Scale9SpriteConfig s9cfg;
};

extern Looper gLooper;
