#pragma once
#include "game_looper_base.h"

struct Button;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	LineStrip ls;

	Camera camera;
};
extern GameLooper gLooper;
