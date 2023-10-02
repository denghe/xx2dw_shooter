#pragma once
#include "game_looper_base.h"

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	LineStrip ls;
};
extern GameLooper gLooper;
