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

	camera.SetScale(2);

	sgabWalls.Init(11, 16, gCellSize.x, gCellSize.y);
	sgcMonsters.Init(11, 16, gCellSize.x);

	rooms.EmplaceShared()->Init({}, 10, 15);

	//for (int i = 0; i < 100; ++i) {
		pumpkins.EmplaceShared()->Init(rooms[0]->GetCenterPos());
		pumpkins.EmplaceShared()->Init(rooms[0]->GetCenterPos());
	//}

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
	for (auto& room : rooms) { room->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {

		for (int i = 0, ie = rooms.len; i < ie; ++i) {
			auto& room = rooms[i];
			room->Draw();
			for (int j = 0, je = room->walls.len; j < je; ++j) {
				yos.Add(room->walls[j]);
			}
		}

		for (int i = 0, ie = pumpkins.len; i < ie; ++i) {
			yos.Add(*pumpkins[i]);
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
