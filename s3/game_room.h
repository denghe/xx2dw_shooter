#pragma once
#include "game_wall.h"

struct Room : Obj {
	int numRows{}, numCols{};
	Vec2<> pos, floorMinXY, floorMaxXY;						// pos: left top

	xx::List<int, int32_t> floors;
	xx::List<Wall, int32_t> walls;

	void Init(Vec2<> const& pos_, int numRows_, int numCols_) {
		numRows = numRows_;
		numCols = numCols_;
		pos = pos_;
		floorMinXY = { pos.x + gCellSize.x, pos.y + gCellSize.y };
		floorMaxXY = { pos.x + (numCols-1) * gCellSize.x, pos.y + (numRows-1) * gCellSize.y };
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
			walls.Emplace().Init(Vec2<>{ pos.x + x * gCellSize.x, pos.y } + gCellSize / 2, gCellSize, fi);
		}

		for (int y = 1; y <= ye - 1; ++y) {
			int posY = pos.y + y * gCellSize.y;
			walls.Emplace().Init(Vec2<>{ pos.x, posY } + gCellSize / 2, gCellSize, 3);
			walls.Emplace().Init(Vec2<>{ pos.x + xe * gCellSize.x, posY } + gCellSize / 2, gCellSize, 5);
		}

		for (int x = 0; x <= xe; ++x) {
			if (x == 0) fi = 0;
			else if (x < xe) fi = 1;
			else fi = 2;
			walls.Emplace().Init(Vec2<>{ pos.x + x * gCellSize.x, pos.y + ye * gCellSize.y } + gCellSize / 2, gCellSize, fi);
		}
	}

	// MoveTo ? sync wall's pos ?

	Vec2<> GetCenterPos() {
		return pos.MakeAdd(numCols * gCellSize.x / 2, numRows * gCellSize.y / 2);
	}
	
	void Draw() const override {
		// draw floor only
		quad.SetScale(gLooper.camera.scale);
		int numFloorRows = numRows - 2, numFloorCols = numCols - 2;
		for (int y = 1; y <= numFloorRows; ++y) {
			for (int x = 1; x <= numFloorCols; ++x) {
				XY p{ pos.x + x * gCellSize.x, pos.y + y * gCellSize.y };
				if (gLooper.camera.InArea(p)) {
					quad.SetPosition(gLooper.camera.ToGLPos(p))
						.SetFrame(gLooper.frames_floor[floors[(y - 1) * numFloorCols + (x - 1)]])
						.Draw();
				}
			}
		}
	}

	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic() {
		while (true) co_yield 0;
	}
};
