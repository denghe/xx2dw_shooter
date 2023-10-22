#pragma once
#include "engine.h"

constexpr GDesign<1280, 720, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// res
	Scale9SpriteConfig s9cfg{ {}, 4, {2,3,2,2} };

	// objs
	xx::Shared<Node> root;
};
extern GameLooper gLooper;
