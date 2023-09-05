#include "pch.h"

void Explosion::Init(XY const& bornPos) {
	Add(MainLogic());
	pos = bornPos;
}
void Explosion::Draw() {
	SetFrame(gLooper.frames_explosion[(int32_t)frameIndex]).Draw();
}
xx::Task<> Explosion::MainLogic() {
	do {
		co_yield 0;
		frameIndex += cFrameInc;
	} while (frameIndex < cFrameMaxIndex);
}
