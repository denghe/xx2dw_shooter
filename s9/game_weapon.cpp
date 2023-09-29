#include "pch.h"

void Weapon_Sword1::Init(xx::Shared<Hero> const& hero_) {
	mainLogic = MainLogic2(); // MainLogic_();
	player = hero_->player;
	hero = hero_;
	frames = &gLooper.frames_weapon;
	frameIndex = cFrameIndex;
	radians = {};
	pos = hero_->weaponPos;
	RotateCirclesAndHitCheck();
	body.SetAnchor(cAnchor);
}

void Weapon_Sword1::Draw() {
	Weapon::Draw();
	LineStrip border;
	border.SetColor({255,0,0,255});
	for (auto& c : circles) {
		auto p = gLooper.camera.ToGLPos(c.pos);
		border.FillCirclePoints({}, c.radius, {}, 16)
			.SetPosition( p )
			.SetScale(scale * gLooper.camera.scale)
			.Draw();
	}
}

void Weapon_Sword1::RotateCirclesAndHitCheck() {
	auto at = AffineTransform::MakePosScaleRadians(pos, { 1,1 }, -radians);
	//printf("%f %f %f\n", pos.x, pos.y, radians);
	for (size_t i = 0; i < std::size(cCircles); i++) {
		circles[i].pos = at.Apply(cCircles[i].pos);
		FindNeighborsCross(gLooper.monstersGrid, circles[i].pos, circles[i].radius, [](auto m) {
			m->Hit(cDamage);
		});
	}
}

// search nearest enemy & aim
xx::Task<> Weapon_Sword1::MainLogic2() {
	while (hero) {
		auto m = FindNearest(gLooper.monstersGrid, gLooper.sgrdd, pos, cSearchDistance);
		if (!m) goto LabEnd;
		{
			auto wm = xx::WeakFromThis(m);
			while (wm) {
				auto v = wm->pos - pos;
				auto r = -std::atan2(-v.y, v.x);
				if (RadiansControl::Step(radians, r, cFrameMaxChangeRadians)) break;
				pos = hero->weaponPos;
				RotateCirclesAndHitCheck();
				co_yield 0;
				if (!hero) co_return;
			}
		}
	LabEnd:
		pos = hero->weaponPos;
		RotateCirclesAndHitCheck();
		co_yield 0;
	}
}
