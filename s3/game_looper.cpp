#include "pch.h"

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
	// load wall texs
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		tp->GetToByPrefix(frames_wall, "wall_");
		frames_floor.push_back(tp->Get("wall_5"));
		tp->GetToByPrefix(frames_pumpkin, "pumpkin_");
		printf("load tex from tp");
	}
	ready = true;

	constexpr int numRows = 220, numCols = 350, numPumpkins = 50000;
	camera.SetOriginal({ numCols * gCellSize.x / 2, numRows * gCellSize.y / 2 });
	camera.SetScale(0.2);
	camera.SetMaxFrameSize(gCellSize.As<float>());

	sgabWalls.Init(numRows + 1, numCols + 1, gCellSize.x, gCellSize.y);
	sgcMonsters.Init(numRows + 1, numCols + 1, gCellSize.x);

	rooms.Emplace().Emplace()->Init({}, numRows, numCols);

	for (int i = 0; i < numPumpkins; ++i) {
		Vec2<> p{ rnd.Next<int>(rooms[0]->floorMinXY.x, rooms[0]->floorMaxXY.x)
		, rnd.Next<int>(rooms[0]->floorMinXY.y, rooms[0]->floorMaxXY.y) };
		pumpkins.Emplace().Emplace()->Init(p);
	}

	while (true) co_yield 0;	// idle for hold memory
}

void GameLooper::Update() {
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
	if (!ready) return;
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
		if (gLooper.camera.InArea(pumpkin.pos)) {
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
