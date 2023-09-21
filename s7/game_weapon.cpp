#include "pch.h"

void Weapon_Sword1::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic();
	pos = hero_->pos;
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	body.SetAnchor(cAnchor);
}

xx::Task<> Weapon_Sword1::MainLogic() {
	while (hero) {
		pos = hero->weaponPos;
		auto v = gLooper.mousePos - gLooper.camera.ToGLPos(pos);
		auto r = -std::atan2(v.y, v.x);
		StepRadians(r, cFrameMaxChangeRadians);

		// mouse aim fire control
		if (gLooper.mouseBtnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = gLooper.nowSecs + cFireDelaySecs;
			r = -radians;
			auto c = std::cos(r);
			auto s = -std::sin(r);
			auto firePos = pos + XY{ c, s } * cFireDistance;
			gLooper.bullets.Emplace().Emplace<Bullet_EyeFire>()->Init(this, firePos, r, c, s);

			// simulate recoil
			auto bak = pos;
			auto steps_5 = std::min(cFireRecoilDelaySecs, cFireDelaySecs) / gDesign.frameDelay / 5;
			auto inc = XY{ c, s } * cFireRecoilSpeed;
			for (int i = 0; i < steps_5 * 2; ++i) {
				pos -= inc;
				co_yield 0;
			}
			for (int i = 0; i < steps_5; ++i) {
				co_yield 0;
			}
			for (int i = 0; i < steps_5 * 2; ++i) {
				pos += inc;
				co_yield 0;
			}
			pos = bak;
		}

		co_yield 0;
	}
}
