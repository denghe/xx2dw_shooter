﻿#pragma once
#include <engine.h>

// circle -- box -- bounce test

constexpr GDesign<1280, 800, 2000> gDesign;


struct Shape {
	XY pos{};
	float radius{}, radians{};
};


struct ShapeCircle : Shape {
	static constexpr float cLifeSpan{ 0.1f };
	static constexpr int32_t cUpdateMultipleTimes{ 1 };
	static constexpr float cSpeed{ 30000.f / gDesign.fps / cUpdateMultipleTimes };
	XY basePos{};
	xx::Task<> mainTask;
	xx::Task<> MainTask();
	void Init(XY const& pos_, float radians_);
	bool Update();
	void Draw();
};


struct ShapeRect : Shape {
	bool dragging{}, lastMBState{};
	XY mouseOffset{};
	xx::FromTo<XY> xy;
	XY GetNearestPoint(XY const& tar);
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
