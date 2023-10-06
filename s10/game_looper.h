#pragma once
#include "engine_engine.h"

int32_t main();
static constexpr GDesign<640, 480, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {

	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	xx::Shared<Node> root;
};
extern GameLooper gLooper;
