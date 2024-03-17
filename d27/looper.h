#pragma once
#include <engine.h>

// circle -- box -- bounce test

constexpr GDesign<1280, 800, 200> gDesign;


struct Shape {
	XY pos{};
	float radius{}, radians{};
};


struct ShapeCircle : Shape {
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	XY basePos{};
	xx::Task<> mainTask;
	xx::Task<> MainTask();
	void Init(float radians_);
	bool Update();
	void Draw();
};


struct ShapeRect : Shape {
	xx::FromTo<XY> xy;
	XY GetNearPoint(XY const& tar);
	void Init();
	bool Update();
	void Draw();
};


struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();
	void Update();
	void Draw();
	bool ready{};

	xx::Ref<Frame> frame_td_shape_rect;
	xx::Ref<Frame> frame_td_shape_trangle;
	xx::Ref<Frame> frame_td_shape_circle;
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;

	xx::ListLink<xx::Shared<ShapeCircle>> shapeCircle;
	xx::Shared<ShapeRect> shapeRect;
};

extern Looper gLooper;
