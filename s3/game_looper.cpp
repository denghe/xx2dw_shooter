﻿#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
		}, nullptr);
}
GameLooper gLooper;


void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();	// font init

	// load wall texs
	{
		DynamicTexturePacker<> dtp;
		dtp.Fill(co_await AsyncLoadTexturesFromUrls({
			"res/wall1.png",
			"res/wall2.png",
			"res/wall3.png",
			"res/wall4.png",
			"res/wall5.png",
			"res/wall6.png",
			"res/wall7.png",
			"res/wall8.png",
			"res/wall9.png",
			"res/pumpkin0.png",
			"res/pumpkin1.png",
			"res/pumpkin2.png",
			"res/pumpkin3.png",
			"res/pumpkin4.png",
			"res/pumpkin5.png",
			"res/pumpkin6.png",
			"res/pumpkin7.png"
		}));
		dtp.GetToByPrefix(frames_wall, "res/wall");
		frames_floor.push_back(dtp.Get("res/wall5.png"));
		dtp.GetToByPrefix(frames_pumpkin, "res/pumpkin");
	}


	ready = true;

	constexpr int numRows = 220, numCols = 350, numPumpkins = 50000;
	camera.SetOriginal({ numCols * gCellSize.x / 2, numRows * gCellSize.y / 2 });
	camera.SetScale(0.2);

	sgabWalls.Init(numRows + 1, numCols + 1, gCellSize.x, gCellSize.y);
	sgcMonsters.Init(numRows + 1, numCols + 1, gCellSize.x);

	rooms.EmplaceShared()->Init({}, numRows, numCols);

	for (int i = 0; i < numPumpkins; ++i) {
		Vec2<> p{ rnd.Next<int>(rooms[0]->floorMinXY.x, rooms[0]->floorMaxXY.x)
		, rnd.Next<int>(rooms[0]->floorMinXY.y, rooms[0]->floorMaxXY.y) };
		pumpkins.EmplaceShared()->Init(p);
	}

	while (true) co_yield 0;	// idle for hold memory
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;
	for (auto& o : rooms) { o->mainLogic(); }
	for (auto& o : pumpkins) { o->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {
		camera.Calc();

		for (int i = 0, ie = rooms.len; i < ie; ++i) {
			auto& room = rooms[i];
			room->Draw();
			for (int j = 0, je = room->walls.len; j < je; ++j) {
				auto& wall = room->walls[j];
				if (gLooper.camera.InArea(wall._sgabPos)) {
					yos.Add(wall);
				}
			}
		}

		for (int i = 0, ie = pumpkins.len; i < ie; ++i) {
			auto& pumpkin = *pumpkins[i];
			if (gLooper.camera.InArea(pumpkin._sgcPos)) {
				yos.Add(pumpkin);
			}
		}

		// todo: add more to ybos ?

		std::sort(yos.buf, yos.buf + yos.len, YObj::Comparer);
		for (int i = 0, ie = yos.len; i < ie; ++i) {
			yos[i].o->Draw();
		}
		yos.Clear();
	}
	fv.Draw(ctc72);       // draw fps at corner
}
