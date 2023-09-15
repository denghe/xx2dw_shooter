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
			"res/wall5.png",	// floor. todo: more
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

	//// make random size rooms
	//for (int i = 0; i < 1000; ++i) {

	//	Vec2<> pos{ rnd.Next<int>(-gMaxRoomWidth_2, gMaxRoomWidth_2)
	//		, rnd.Next<int>(-gMaxRoomHeight_2, gMaxRoomHeight_2) };

	//	Vec2<> size{ rnd.Next<int>(3, 20), rnd.Next<int>(3, 15) };

	//	auto&& room = rooms.EmplaceShared();
	//	room->Init(pos, size);

	//	co_yield 0;
	//}


	rooms.EmplaceShared()->Init({}, 10, 15);


	// todo: move?

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

		for (auto& room : rooms) {
			room->Draw();
			for (auto& w : room->walls) {
				ybos.Add(YBoxObj{ w.pos.y, &w });
			}
		}

		// todo: add more to ybos ?

		std::sort(ybos.buf, ybos.buf + ybos.len, YBoxObj::Comparer);
		for (auto& o : ybos) {
			o.bo->Draw();
		}
		ybos.Clear();
	}
	fv.Draw(ctc72);       // draw fps at corner
}

///*****************************************************************************************************/
///*****************************************************************************************************/
//
//void Room::Draw() {
//
//	// todo: think about order by Y. group by row draw bg first ? every tile is object ?? except floor ?
//
//	auto pixelSize_2 = size * gRoomCellSize / 2;
//
//	float basePosX = (int)pos.x * gRoomCellSize - pixelSize_2.x;
//	float basePosY = (int)-pos.y * gRoomCellSize + pixelSize_2.y;
//
//	for (int y = 0, ye = size.y - 1; y <= ye; ++y) {
//		for (int x = 0, xe = size.x - 1; x <= xe; ++x) {
//
//			XY p{ float(basePosX + x * gRoomCellSize), basePosY - y * gRoomCellSize };
//
//			int fi;
//			if (y == 0) {
//				if (x == 0) fi = 6;
//				else if (x < xe) fi = 7;
//				else fi = 8;
//			} else if (y < ye) {
//				if (x == 0) fi = 3;
//				else if (x < xe) fi = 4;
//				else fi = 5;
//			} else {
//				if (x == 0) fi = 0;
//				else if (x < xe) fi = 1;
//				else fi = 2;
//			}
//
//			body.SetPosition(p * gLooper.camera.scale)
//				.SetFrame(gLooper.frames_walls[fi])
//				.SetScale(gLooper.camera.scale)
//				.Draw();
//		}
//	}
//}
//
//xx::Task<> Room::MainLogic() {
//	while (true) {
//
//		co_yield 0;
//	}
//}
