#pragma once
#include "engine.h"

constexpr GDesign<1280, 800, 240> gDesign;

static constexpr int gRoomCellSize = 16;
static constexpr int gMaxRoomWidth = (int)gDesign.width / gRoomCellSize;
static constexpr int gMaxRoomHeight = (int)gDesign.height / gRoomCellSize;
static constexpr int gMaxRoomWidth_2 = gMaxRoomWidth / 2;
static constexpr int gMaxRoomHeight_2 = gMaxRoomHeight / 2;

struct Room;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Ref<Frame>> frames_walls;

	// objs
	xx::List<xx::Shared<Room>, int32_t> rooms;

	bool hasCross{};

	Camera camera;
};
extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Room {

	Quad body;
	XY pos, size;
	XY GetMinXY() const;
	XY GetMaxXY() const;
	bool Intersects(Room const& o) const;

	void Init(XYi const& pos_, XYi const& size_);
	void Draw();
	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic();
};
