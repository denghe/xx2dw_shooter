#include "pch.h"

void Monster3::Init(XY const& bornPos) {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
	scale = {};
	pos = bornPos;
	GridInit();
}
void Monster3::Draw() {
	SetFrame(gLooper.frames_monster_3[(int32_t)frameIndex]).Draw();
}
xx::Task<> Monster3::MainLogic() {
	//GridUpdate();
	while (scale.x < 1.f) {
		scale.x += 0.1f;
		scale.y += 0.1f;
		co_yield 0;
	}
	scale = { 1, 1 };

	while (--life > 0) {
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}
		co_yield 0;
	}

	while (scale.x > 0.1f) {
		scale.x -= 0.1f;
		scale.y -= 0.1f;
		co_yield 0;
	}
}
