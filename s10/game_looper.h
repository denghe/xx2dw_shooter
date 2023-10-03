#pragma once
#include "engine_engine.h"

int32_t main();

constexpr GDesign<640, 480, 60> gDesign;

struct Node;
struct ZNode;

struct GameLooper : Engine<GameLooper> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;

	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	xx::Weak<Node> mouseHandler{};
	// register mouse event handler. use list double link? + insert funciton ? use space grid index ? every time sort by z ?
	// use priority queue ? 
	// need heap container sort by z? 
	std::set<xx::Weak<Node>> mouseDownHandlers;
	std::set<xx::Weak<Node>> mouseMoveHandlers;
	std::set<xx::Weak<Node>> mouseUpHandlers;
	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e);
	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e);

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	xx::Shared<Node> root;
	xx::List<ZNode> tmpZNodes;

};
extern GameLooper gLooper;

/**********************************************************************************************/
/**********************************************************************************************/

struct Button : Node {
	std::function<void()> onClicked;
	XY minXY, maxXY;	// todo: fill

	// todo: color ? colormut ? anchor ?
	void Init(int z_, XY const& position_, float texScale_, xx::Shared<Frame> frame_, UVRect const& center_, std::u32string_view const& txt_) {
		z = z_;
		position = position_;

		auto lbl = xx::Make<Label>();
		lbl->Init(z + 2, position, {0.5, 0.5}, RGBA8_White, txt_);	// todo: scale
		children.Add(lbl);

		XY cornerSize{ float(frame_->textureRect.w - center_.w), float(frame_->textureRect.h - center_.h) };
		XY padding{ 20, 5 };
		size = lbl->size + cornerSize + padding;
		//minXY = 

		auto bg = xx::Make<Scale9Sprite>();
		bg->Init(z + 1, position, texScale_, (lbl->size + cornerSize + padding) / texScale_, std::move(frame_), center_, { 0x5f, 0x15, 0xd9, 0xff });
		children.Add(bg);
	}


	// todo: mouse click handle
};
