#include "pch.h"

void HandWeapon_Sword1::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic();
	pos = hero_->pos;
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	body.SetAnchor(cAnchor);
}

//void HandWeapon_Sword1::Fire() {
//
//}

xx::Task<> HandWeapon_Sword1::MainLogic() {
	while (hero) {
		pos = hero->weaponPos;
		auto v = gLooper.mousePos - gLooper.camera.ToGLPos(pos);
		auto r = -std::atan2(v.y, v.x);
		StepRadians(r, cFrameMaxChangeRadians);
		firePos = pos + XY{ std::cos(r), std::sin(r) } * cFireDistance;

		// mouse aim fire control
		if (gLooper.mouseBtnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = gLooper.nowSecs + cFireDelaySecs;
			gLooper.bullets.Emplace().Emplace<Bullet_EyeFire>()->Init(this);
		}

		co_yield 0;
	}
}
