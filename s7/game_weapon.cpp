#include "pch.h"

void Weapon_1_Sword::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic2(); // MainLogic_();
	pos = hero_->pos;
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	body.SetAnchor(cAnchor);
}

// mouse aim fire control
xx::Task<> Weapon_1_Sword::MainLogic_() {
	while (hero) {
		pos = hero->weaponPos;
		auto v = gLooper.mouse.pos - gLooper.camera.ToGLPos(pos);
		auto r = -std::atan2(v.y, v.x);
		RadiansControl::Step(radians, r, cFrameMaxChangeRadians);

		if (gLooper.mouse.btnStates[0] && nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = gLooper.nowSecs + cFireDelaySecs;
			r = -radians;
			auto c = std::cos(r);
			auto s = -std::sin(r);
			auto firePos = pos + XY{ c, s } * cFireDistance;
			//switch (gLooper.rnd.Next<int>(2)) {
			//case 0:
			//	gLooper.bullets.Emplace().Emplace<Bullet_EyeFire>()->Init(this, firePos, r, c, s);
			//	break;
			//case 1:
			//	gLooper.bullets.Emplace().Emplace<Bullet_EyeFireBlue>()->Init(this, firePos, r, c, s);
			//	break;
			//case 2:
			//	gLooper.bullets.Emplace().Emplace<Bullet_Fireball>()->Init(this, firePos, r, c, s);
			//	break;
			//}
			gLooper.bullets.Emplace().Emplace<Bullet_Fireball>()->Init(this, firePos, r, c, s);

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

// search nearest enemy & auto fire
xx::Task<> Weapon_1_Sword::MainLogic2() {
	while (hero) {
		co_yield 0;
		pos = hero->weaponPos;
	LabBegin:
		auto m = FindNearest(gLooper.monstersGrid, gLooper.sgrdd, pos, cSearchDistance);
		if (!m) continue;
		auto wm = xx::WeakFromThis(m);
		while (true) {
			auto v = wm->pos - pos;
			auto r = -std::atan2(-v.y, v.x);
			if (RadiansControl::Step(radians, r, cFrameMaxChangeRadians)) break;
			co_yield 0;
			if (!hero) co_return;
			pos = hero->weaponPos;
			if (!wm) goto LabBegin;
		}

		// fire
		if (nextFireSecs <= gLooper.nowSecs) {
			nextFireSecs = gLooper.nowSecs + cFireDelaySecs;
			auto r = -radians;
			auto c = std::cos(r);
			auto s = -std::sin(r);
			auto firePos = pos + XY{ c, s } *cFireDistance;
			gLooper.bullets.Emplace().Emplace<Bullet_Fireball>()->Init(this, firePos, r, c, s);

			// simulate recoil
			auto bak = pos;
			auto steps_5 = std::min(cFireRecoilDelaySecs, cFireDelaySecs) / gDesign.frameDelay / 5;
			auto inc = XY{ c, s } *cFireRecoilSpeed;
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
	}
}
