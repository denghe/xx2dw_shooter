#include "pch.h"

void Weapon_1_Sword::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic();
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	radians = {};
	pos = hero_->weaponPos;
	RotateCirclesAndHitCheck();
	body.SetAnchor(cAnchor);
}

void Weapon_1_Sword::Draw() {
	Weapon::Draw();
	//LineStrip border;
	//border.SetColor({255,0,0,255});
	//for (auto& c : circles) {
	//	auto p = gLooper.camera.ToGLPos(c.pos);
	//	border.FillCirclePoints({}, c.radius, {}, 16)
	//		.SetPosition( p )
	//		.SetScale(scale * gLooper.camera.scale)
	//		.Draw();
	//}
}

void Weapon_1_Sword::RotateCirclesAndHitCheck() {
	auto trans = AffineTransform::MakePosScaleRadians(pos, { 1,1 }, -radians);
	for (size_t i = 0; i < std::size(cCircles); i++) {
		circles[i].pos = trans(cCircles[i].pos);
		FindNeighborsCross(gLooper.monstersGrid, circles[i].pos, circles[i].radius, [&](auto m) {
			m->Hit(cDamage, hero);
		});
	}
}

// search nearest enemy & aim
xx::Task<> Weapon_1_Sword::MainLogic() {
	while (hero) {
		auto m = FindNearest(gLooper.monstersGrid, gLooper.sgrdd, pos, cSearchDistance);
		if (m) {
			auto wm = xx::WeakFromThis(m);
			while (wm) {
				auto v = wm->pos - pos;
				auto r = -std::atan2(-v.y, v.x);
				if (RotateControl::Step(radians, r, cFrameMaxChangeRadians)) break;
				pos = hero->weaponPos;
				RotateCirclesAndHitCheck();
				co_yield 0;
				if (!hero) co_return;
			}
		}
		pos = hero->weaponPos;
		RotateCirclesAndHitCheck();
		co_yield 0;
	}
}






void Weapon_19_Stuff::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic();
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	radians = {};
	pos = hero_->weaponPos;
	body.SetAnchor(cAnchor);
}

// search nearest enemy & aim
xx::Task<> Weapon_19_Stuff::MainLogic() {
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
			if (RotateControl::Step(radians, r, cFrameMaxChangeRadians)) break;
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
