#include "pch.h"

void Bullet_EyeFire::Init(HandWeapon* hw, XY const& pos_, float r, float c, float s) {
	mainLogic = MainLogic();
	radius = cRadius;
	pos = pos_;
	radians = -r;
	inc = XY{ c, -s } * cSpeed;
	player = hw->player;
	frames = &gLooper.frames_eye_fire;
	frameIndex = 0;
	body.SetAnchor(cAnchor);
}

xx::Task<> Bullet_EyeFire::MainLogic() {
	for (float life = 1; life > 0; life -= cLifeDelta) {
		ForwardFrame(cFrameInc * speed / cSpeed, cFrameMaxIndex);

		pos += inc;
		co_yield 0;
	}
}
