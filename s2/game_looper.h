﻿#pragma once
#include "game_looper_base.h"

static constexpr int gRoomCellSize = 16;
static constexpr int gMaxRoomWidth = gDesign.width / gRoomCellSize;
static constexpr int gMaxRoomHeight = gDesign.height / gRoomCellSize;
static constexpr int gMaxRoomWidth_2 = gMaxRoomWidth / 2;
static constexpr int gMaxRoomHeight_2 = gMaxRoomHeight / 2;

struct Room;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Shared<Frame>> frames_walls;

	// objs
	xx::List<xx::Shared<Room>, int32_t> rooms;
};
extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Room {

	Quad body;
	Vec2<> pos, size;

	void Init(Vec2<> const& pos_, Vec2<> const& size_);
	void Draw();
	xx::Task<> mainLogic;
	xx::Task<> MainLogic();
};
