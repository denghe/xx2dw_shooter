#pragma once
#include <engine.h>

// circle -- box -- bounce test

constexpr GDesign<1280, 800, 60> gDesign;


struct Shape {
	XY pos{};
	float radius;
};


struct ShapeCircle : Shape {
	XY basePos{};
	xx::Task<> mainTask;
	xx::Task<> MainTask();
	void Init();
	void Update();
	void Draw();
};


struct ShapeRect : Shape {
	void Init();
	void Update();
	void Draw();
};


struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> DrawTask();

	xx::Ref<Frame> frame_td_shape_rect;
	xx::Ref<Frame> frame_td_shape_trangle;
	xx::Ref<Frame> frame_td_shape_circle;
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;

	xx::Shared<ShapeCircle> shapeCircle;
	xx::Shared<ShapeRect> shapeRect;
};

extern Looper gLooper;
