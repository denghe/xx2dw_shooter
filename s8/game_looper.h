#pragma once
#include "game_looper_base.h"

struct Button;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	xx::Shared<Frame> frame_button;

	// objs
	xx::ListLink<xx::Shared<Button>, int32_t> buttons;	// todo: special container for ui ?

	Camera camera;
};
extern GameLooper gLooper;


struct Button {
	XY pos{}, size{};
	std::string txt;	// todo: change to TinyFrame array?
	std::function<void()> onClicked;
	void Init(XY const& pos_, XY const& size_, std::string_view const& txt_);
	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic();
	void Draw();
};
