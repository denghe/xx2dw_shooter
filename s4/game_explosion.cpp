#include "pch.h"

void Explosion::Init(XY const& bornPos) {
	Add(MainLogic());
	pos = bornPos;
	auto& c = gLooper.camera;
	body.SetScale(cScale * c.scale);
}

void Explosion::Draw() {
	auto& c = gLooper.camera;
	body.TrySetFrame(gLooper.frames_explosion[(int32_t)frameIndex]).SetPosition(c.ToGLPos(pos)).Draw();
}

xx::Task<> Explosion::MainLogic() {
	do {
		co_yield 0;
		frameIndex += cFrameInc;
	} while (frameIndex < cFrameMaxIndex);
}
