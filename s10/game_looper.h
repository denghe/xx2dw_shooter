#pragma once
#include "game_looper_base.h"

struct Node;
struct ZNode;

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
	xx::Shared<Node> root;
	xx::List<ZNode> tmpZNodes;
};
extern GameLooper gLooper;

/**********************************************************************************************/
/**********************************************************************************************/

struct Button : Node {
	std::function<void()> onClicked;

	void Init(int z_, XY const& position_, float texScale_, xx::Shared<Frame> frame_, UVRect const& center_, std::string_view const& txt_) {
		z = z_;
		position = position_;

		auto lbl = xx::Make<Label>();
		lbl->Init(z + 2, position + XY{ 3, 3 }, txt_);

		auto bg = xx::Make<Scale9Sprite>();
		auto siz = lbl->size + XY{ float(frame_->textureRect.w - center_.w), float(frame_->textureRect.h - center_.h) };
		bg->Init(z + 1, position, texScale_, siz / texScale_, std::move(frame_), center_);

		children.Add(bg);
		children.Add(lbl);
	}
};
