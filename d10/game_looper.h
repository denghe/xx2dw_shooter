#pragma once
#include "engine.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// res
	Scale9SpriteConfig s9cfg{ {}, {4,4}, {2,3,2,2}, { 0x5f, 0x15, 0xd9, 0xff } };

	// objs
	xx::Shared<Node> root;
};
extern GameLooper gLooper;
