#pragma once
#include "game_looper.h"

struct Wall : BoxObj {
	static constexpr float cScale{ 1 };


	void Init(Vec2<> const& pos, Vec2<> const& size, int frameIndex) {
		
		SGABInit(gLooper.sgabWalls);
		SGABSetPosSiz(pos, size);
		SGABAdd();

		quad.SetFrame(gLooper.frames_wall[frameIndex]);
	}

	operator YObj() const {
		return { _sgabPos.y - gCellSize.y / 2, this };
	}

	void Draw() const override {
		auto scale = cScale * gLooper.camera.scale;
		quad.SetScale(scale)
			.SetPosition(_sgabPos.As<float>().MakeFlipY() * scale)
			.Draw();
	}
};
