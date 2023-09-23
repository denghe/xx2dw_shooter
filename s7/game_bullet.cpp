#include "pch.h"

void Bullet_EyeFire::Init(Weapon* hw, XY const& pos_, float r, float c, float s) {
	mainLogic = MainLogic_();
	radius = cRadius;
	pos = pos_;
	radians = -r;
	inc = XY{ c, s } * cSpeed;
	player = hw->player;
	frames = &gLooper.frames_eye_fire;
	frameIndex = 0;
	body.SetAnchor(cAnchor);
}

xx::Task<> Bullet_EyeFire::MainLogic_() {
	for (float life = 1; life > 0; life -= cLifeDelta) {
		pos += inc;

		// check hit monsters
		if (auto r = Monster::FindNeighbor(gLooper.monstersGrid, pos, radius)) {
			r->Hit(damage);		// r maybe deleted
			co_return;
		}

		// todo: check walls

		ForwardFrame(cFrameInc * speed / cSpeed, cFrameMaxIndex);
		co_yield 0;
	}
}
