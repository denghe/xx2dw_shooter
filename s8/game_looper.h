#pragma once
#include "game_looper_base.h"

struct DragCircle {
	void Init(XY const& pos, float radius, int32_t segments);
	void Draw();
	xx::Task<> mainTask{ MainTask() };
	xx::Task<> MainTask();

	XY pos{};
	float radius{}, speed{};
	LineStrip border;
};

struct Poly {
	std::array<XY, 4> vertices;
	LineStrip border;
	void Init();
	void Draw();
};


struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	xx::Task<> MainTask();
};
extern GameLooper gLooper;
