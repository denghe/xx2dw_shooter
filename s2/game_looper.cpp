#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
		}, nullptr);
}
GameLooper gLooper;


xx::Task<> GameLooper::MainTask() {
	camera.SetScale(0.25);

	// load wall texs
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		tp->GetToByPrefix(frames_walls, "wall_");
	}


	ready = true;

	// make random size rooms
	for (int i = 0; i < 1000; ++i) {

		Vec2<> pos{ rnd.Next<int>(-gMaxRoomWidth_2, gMaxRoomWidth_2)
			, rnd.Next<int>(-gMaxRoomHeight_2, gMaxRoomHeight_2) };

		Vec2<> size{ rnd.Next<int>(3, 20), rnd.Next<int>(3, 15) };

		auto&& room = rooms.Emplace().Emplace();
		room->Init(pos, size);

		co_yield 0;
	}

	// todo: move?

	while (true) co_yield 0;	// idle for hold memory
}

void GameLooper::Update() {
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;
	hasCross = {};
	for (auto& room : rooms) { room->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {
		for (auto& room : rooms) { room->Draw(); }

		if (!hasCross) {
			ctcDefault.Draw({ -gEngine->windowWidth_2, gEngine->windowHeight_2 - ctcDefault.canvasHeight_2 }, "calculate done.");
		} else {
			ctcDefault.Draw({ -gEngine->windowWidth_2, gEngine->windowHeight_2 - ctcDefault.canvasHeight_2 }, "keyboard Z X zoom.");
		}
	}
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Room::Init(Vec2<> const& pos_, Vec2<> const& size_) {
	pos = pos_.As<float>();
	size = size_.As<float>();

	body.SetAnchor({});
}

void Room::Draw() {

	auto pixelSize_2 = size * gRoomCellSize / 2;

	float basePosX = (int)pos.x * gRoomCellSize - pixelSize_2.x;
	float basePosY = (int)-pos.y * gRoomCellSize + pixelSize_2.y;

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

			body.SetPosition(p * gLooper.camera.scale)
				.SetFrame(gLooper.frames_walls[fi])
				.SetScale(gLooper.camera.scale)
				.Draw();
		}
	}
}

XY Room::GetMinXY() const {
	return pos - (size + 1) / 2;
}
XY Room::GetMaxXY() const {
	return pos + (size + 1) / 2;
}
bool Room::Intersects(Room const& o) const {
	auto minXY = GetMinXY();
	auto maxXY = GetMaxXY();
	auto oMinXY = o.GetMinXY();
	auto oMaxXY = o.GetMaxXY();
	return !(maxXY.x < oMinXY.x || oMaxXY.x < minXY.x ||
		maxXY.y < oMinXY.y || oMaxXY.y < minXY.y);
}

xx::Task<> Room::MainLogic() {
	while (true) {
		XY combineForce{};
		int numCross{};

		for (auto& room : gLooper.rooms) {
			if (room.pointer == this) continue;
			if (!Intersects(*room)) continue;

			++numCross;
			float r;
			if (pos == room->pos) {
				r = gEngine->rnd.Next<float>(M_PI * 2);
			} else {
				auto d = (pos - room->pos).As<float>();
				r = std::atan2(d.y, d.x);
			}
			combineForce += XY{ std::cos(r), std::sin(r) };
		}

		if (numCross) {
			gLooper.hasCross = true;
			pos += combineForce.MakeNormalize();
		}

		co_yield 0;
	}
}
