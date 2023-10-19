#pragma once
#include "game_looper.h"

struct Sprite {
	Quad body;
	XY pos;
	float radius{}, radians{};
	float frameIndex{};
	std::vector<xx::Ref<Frame>>const* frames{};
	xx::Task<> mainLogic;

	void Draw() {
		body.SetScale(gLooper.camera.scale)
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.SetRotate(radians)
			.SetFrame((*frames)[(size_t)frameIndex])
			.Draw();
	}
};
