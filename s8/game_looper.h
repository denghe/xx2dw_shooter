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
	std::vector<xx::Shared<Frame>> frames_button;

	// objs
	xx::ListLink<xx::Shared<Button>, int32_t> buttons;

	Camera camera;
};
extern GameLooper gLooper;
