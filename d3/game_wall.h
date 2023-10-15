#pragma once
#include "game_looper.h"

struct Wall : BoxObj {
	void Init(Vec2<> const& pos, Vec2<> const& size, int frameIndex) {
		
		SGABAdd(gLooper.sgabWalls, pos, size);

		quad.SetFrame(gLooper.frames_wall[frameIndex]);
	}

	operator YObj() const {
		return { _sgabPos.y - gCellSize.y / 2, this };
	}

	void Draw() const override {
		quad.SetScale(gLooper.camera.scale)
			.SetPosition(gLooper.camera.ToGLPos(_sgabPos))
			.Draw();
	}
};
