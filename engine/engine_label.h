#pragma once
#include "engine_node.h"

struct Label : Node {
	std::u32string txt;

	void Init(int z_, XY const& position_, std::string_view const& txt_) {
		z = z_;
		position = position_;
		txt = xx::StringU8ToU32(txt_);
		anchor = {};	// todo: do not use ctc.Draw
		size.x = gEngineBaseEx->ctcDefault.Measure(txt);
		size.y = gEngineBaseEx->ctcDefault.canvasHeight;
	}

	virtual void Draw() override {
		gEngineBaseEx->ctcDefault.Draw(at.Apply(position) - XY{ size.x / 2, 0 }, txt);
	}
};
