#pragma once
#include "game_looper_base.h"

struct BoxObj;
struct Room;
struct Wall;
struct Pumpkin;

struct YBoxObj {
	float y;
	BoxObj* bo;
	inline static bool Comparer(YBoxObj const& a, YBoxObj const& b) {
		return a.y < b.y;
	}
};

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Shared<Frame>> frames_floor, frames_wall, frames_pumpkin;

	// phys
	//SpaceGridAB<Block> sgabBlocks;

	// objs
	xx::List<xx::Shared<Room>, int32_t> rooms;	// walls
	xx::List<xx::Shared<Pumpkin>, int32_t> pumpkins;

	// tmp for draw order by Y
	xx::List<YBoxObj, int32_t> ybos;

	Camera camera;
};
extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct BoxObj {
	XY pos{}, size{}, radius{}, minXY{}, maxXY{};
	Quad quad;

protected:
	void Init(XY const& pos_, XY const& size_) {
		pos = pos_;
		size = size_;
		radius = (size_ + 1) / 2;
		minXY = pos_ - radius;
		maxXY = pos_ + radius;
	}
public:

	bool Intersects(BoxObj const& o) const {
		return !(maxXY.x < o.minXY.x || o.maxXY.x < minXY.x || maxXY.y < o.minXY.y || o.maxXY.y < minXY.y);
	}

	virtual void Draw() = 0;
	virtual ~BoxObj() {}
};

struct Wall : BoxObj {
	static constexpr float cScale{ 1 };

	void Init(XY const& pos_, XY const& size_, int frameIndex) {
		BoxObj::Init(pos_, size_);
		quad.SetAnchor({0, 1}).SetFrame(gLooper.frames_wall[frameIndex]);
	}

	void Draw() override {
		auto scale = cScale * gLooper.camera.scale;
		quad.SetScale(scale)
			.SetPosition(pos.MakeFlipY() * scale)
			.Draw();
	}
};

struct Room : BoxObj {
	static constexpr XY cCellSize{ 16, 16 };

	int numRows{}, numCols{};
	xx::List<int, int32_t> floors;
	xx::List<Wall, int32_t> walls;

	void Init(XY const& pos_, int numRows_, int numCols_) {
		numRows = numRows_;
		numCols = numCols_;
		BoxObj::Init(pos_, { numCols * cCellSize.x, numRows * cCellSize.y });
		InitFloors();
		InitWalls();
		quad.SetAnchor({0, 1}).SetFrame(gLooper.frames_floor.back());
	}

	void InitFloors() {
		int numFloorRows = numRows - 2, numFloorCols = numCols - 2;
		floors.Resize(numFloorRows * numFloorCols);
		for (int y = 1; y <= numFloorRows; ++y) {
			for (int x = 1; x <= numFloorCols; ++x) {
				floors[(y - 1) * numFloorCols + (x - 1)] = 0;	// todo: more floor frame types
			}
		}
	}

	void InitWalls() {
		walls.Reserve(numRows * numCols - floors.len);
		int fi;
		int xe = numCols - 1;
		int ye = numRows - 1;

		for (int x = 0; x <= xe; ++x) {
			if (x == 0) fi = 6;
			else if (x < xe) fi = 7;
			else fi = 8;
			walls.Emplace().Init({ minXY.x + x * cCellSize.x, minXY.y }, cCellSize, fi);
		}

		for (int y = 1; y <= ye - 1; ++y) {
			float posY = minXY.y + y * cCellSize.y;
			walls.Emplace().Init({ minXY.x, posY }, cCellSize, 3);
			walls.Emplace().Init({ minXY.x + xe * cCellSize.x, posY }, cCellSize, 5);
		}

		for (int x = 0; x <= xe; ++x) {
			if (x == 0) fi = 0;
			else if (x < xe) fi = 1;
			else fi = 2;
			walls.Emplace().Init({ minXY.x + x * cCellSize.x, minXY.y + ye * cCellSize.y }, cCellSize, fi);
		}
	}

	// MoveTo ? sync wall's pos ?

	void Draw() override {
		// draw floor only
		auto scale = Wall::cScale * gLooper.camera.scale;
		XY basePos = minXY.MakeFlipY() * scale;
		quad.SetScale(scale);
		int numFloorRows = numRows - 2, numFloorCols = numCols - 2;
		for (int y = 1; y <= numFloorRows; ++y) {
			for (int x = 1; x <= numFloorCols; ++x) {
				XY p{ basePos.x + x * cCellSize.x * scale, basePos.y - y * cCellSize.y * scale };
				quad.SetPosition(p)
					.TrySetFrame(gLooper.frames_floor[floors[(y - 1) * numFloorCols + (x - 1)]])
					.Draw();
			}
		}
	}

	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic() {
		while (true) co_yield 0;
	}
};

struct Pumpkin : BoxObj {
	static constexpr XY cSize{ 16, 24 };
	static constexpr float cScale{ 1 };
	constexpr static float cFrameInc{ 24 / gDesign.fps };
	constexpr static float cFrameMaxIndex{ 4.f };

	float frameIndex{};
	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic() {
		while (true) {
			frameIndex += cFrameInc;					// step frame anim
			if (frameIndex >= cFrameMaxIndex) {			// loop step
				frameIndex -= cFrameMaxIndex;
			}
			co_yield 0;
		}
	}

	void Init(XY const& pos_) {
		BoxObj::Init(pos_, cSize);
		quad.SetAnchor({0.5, 0});
	}

	void Draw() override {
		quad.TrySetFrame(gLooper.frames_pumpkin[(int32_t)frameIndex])
			.SetScale(cScale * gLooper.camera.scale)
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.Draw();
	}
};
