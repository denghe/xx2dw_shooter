#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

static constexpr Vec2<> gCellSize{ 16, 16 };
static constexpr int32_t gGridNumCols = 128, gGridNumRows = 128;
static constexpr Vec2<> gGridBasePos{ gCellSize.x * gGridNumCols / 2, gCellSize.y * gGridNumRows / 2 };

struct Obj;
struct BoxObj;
struct CircleObj;
struct Room;
struct Wall;
struct Pumpkin;

struct YObj {
	int y;
	Obj const* o;
	inline static bool Comparer(YObj const& a, YObj const& b) {
		return a.y < b.y;
	}
};

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Ref<Frame>> frames_floor, frames_wall, frames_pumpkin;

	// phys
	SpaceGridAB<BoxObj> sgabWalls;
	SpaceGridC<CircleObj> sgcMonsters;

	// objs
	xx::List<xx::Shared<Room>, int32_t> rooms;	// walls
	xx::List<xx::Shared<Pumpkin>, int32_t> pumpkins;

	// tmp for draw order by Y
	xx::List<YObj, int32_t> yos;

	Camera camera;
};
extern GameLooper gLooper;
