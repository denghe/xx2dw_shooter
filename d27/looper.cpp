#include "pch.h"
#include "looper.h"

xx::Task<> Looper::DrawTask() {

	// begin load / download textures
	std::string resRoot = "map_td3/";
	std::string picRoot = resRoot + "pics/";

	// prepare
	ffs.emplace_back(picRoot + "td_shape_rect.png", &frame_td_shape_rect);
	ffs.emplace_back(picRoot + "td_shape_trangle.png", &frame_td_shape_trangle);
	ffs.emplace_back(picRoot + "td_shape_circle.png", &frame_td_shape_circle);

	// load / download
	for (auto& ff : ffs) {
#ifdef __EMSCRIPTEN__
		*ff.second = co_await AsyncLoadFrameFromUrl(ff.first);
#else
		*ff.second = LoadFrame(ff.first);
#endif
	}

	// batch combine textures
	auto ok = DynamicTexturePacker<512>::Pack(ffs);
	assert(ok);


	// init shapes
	shapeCircle.Emplace()->Init();
	shapeRect.Emplace()->Init();

	// loop
	while (true) {
		shapeCircle->Update();
		shapeRect->Update();

		shapeCircle->Draw();
		shapeRect->Draw();
		co_yield 0;
	}
}


void ShapeCircle::Init() {
	mainTask = MainTask();
	basePos = { -400, 0 };
	radius = 150;
	mainTask();
}

xx::Task<> ShapeCircle::MainTask() {
	while (true) {
		for (float i = 0; i < 300; ++i) {
			pos = basePos + i;
			co_yield 0;
		}
		for (float i = 300; i > 0; --i) {
			pos = basePos + i;
			co_yield 0;
		}
	}
}

void ShapeCircle::Update() {
	mainTask();
}

void ShapeCircle::Draw() {
	Quad().SetFrame(gLooper.frame_td_shape_circle)
		.SetScale(radius / 16.f)
		.SetPosition(pos)
		.Draw();
}



void ShapeRect::Init() {
	pos = {};
	radius = 120;
}

void ShapeRect::Update() {
}

void ShapeRect::Draw() {
	Quad().SetFrame(gLooper.frame_td_shape_rect)
		.SetScale(radius / 16.f)
		.SetPosition(pos)
		.Draw();
}
