#include "pch.h"

void Bullet_EyeFire::Init(HandWeapon* hw) {
	mainLogic = MainLogic();
	radius = cRadius;
	pos = hw->firePos;
	radians = hw->radians;
	player = hw->player;
	frames = &gLooper.frames_eye_fire;
	frameIndex = 0;
	body.SetAnchor(cAnchor);
}

xx::Task<> Bullet_EyeFire::MainLogic() {
	float r = -radians + M_PI * 4;
	XY inc = XY{ std::cos(r), -std::sin(r) } * cSpeed;
	for (float life = 1; life > 0; life -= cLifeDelta) {
		pos += inc;
		co_yield 0;
	}
}
