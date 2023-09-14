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
		auto texs = co_await AsyncLoadTexturesFromUrls({
			"res/wall1.png",
			"res/wall2.png",
			"res/wall3.png",
			"res/wall4.png",
			"res/wall5.png",
			"res/wall6.png",
			"res/wall7.png",
			"res/wall8.png",
			"res/wall9.png"
		});
		DynamicTexturePacker<> dtp;
		dtp.Fill(texs);
		frames_walls = std::move(dtp.frames);
	}

	ready = true;

	// make random size rooms
	for (int i = 0; i < 20; ++i) {

		Vec2<> pos{ rnd.Next<int>(-gMaxRoomWidth_2, gMaxRoomWidth_2)
			, rnd.Next<int>(-gMaxRoomHeight_2, gMaxRoomHeight_2) };

		Vec2<> size{ rnd.Next<int>(3, 20), rnd.Next<int>(3, 15) };

		auto&& room = rooms.EmplaceShared();
		room->Init(pos, size);
	}

	// todo: move?

	while (true) co_yield 0;	// idle for hold memory
}

void GameLooper::Update() {
	fv.Update();
	if (!ready) return;
	for (auto& room : rooms) { room->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {
		for (auto& room : rooms) { room->Draw(); }
	}
	fv.Draw(ctc72);       // draw fps at corner
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Room::Init(Vec2<> const& pos_, Vec2<> const& size_) {
	pos = pos_;
	size = size_;

	body.SetAnchor({});
}

void Room::Draw() {
	float basePosX = pos.x * gRoomCellSize;
	float basePosY = -pos.y * gRoomCellSize;
	for (int y = 0, ye = size.y - 1; y <= ye; ++y) {
		for (int x = 0, xe = size.x - 1; x <= xe; ++x) {

			XY p{ float(basePosX + x * gRoomCellSize), basePosY - y * gRoomCellSize };

			int fi;
			if (y == 0) {
				if (x == 0) fi = 6;
				else if (x < xe) fi = 7;
				else fi = 8;
			} else if (y < ye) {
				if (x == 0) fi = 3;
				else if (x < xe) fi = 4;
				else fi = 5;
			} else {
				if (x == 0) fi = 0;
				else if (x < xe) fi = 1;
				else fi = 2;
			}

			body.SetPosition(p)
				.SetFrame(gLooper.frames_walls[fi])
				.Draw();
		}
	}
}

xx::Task<> Room::MainLogic() {
	while (true) co_yield 0;
}
