﻿#pragma once
#include "engine_engine_desktop.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// res
	xx::Shared<Frame> frame_button;
	static constexpr float buttonTextureScale{ 4 };
	static constexpr UVRect buttonUvRect{ 2,3,2,2 };

	// objs
	xx::Shared<Node> root;
};
extern GameLooper gLooper;
