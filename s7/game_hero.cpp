#include "pch.h"

void Hero_Pumpkin::Init(xx::Shared<Player> const& player_, XY const& bornPos) {
	mainLogic = MainLogic_();
	idle = Idle_ScaleY();
	player = player_;
	radius = cRadius;
	pos = bornPos;
	speed = cSpeed;
	eatExperienceDistance = cEatExperienceDistance;
	frames = &gLooper.frames_pumpkin;
	body.SetAnchor(cAnchor);
	weapon.Emplace<Weapon_Sword1>()->Init(xx::SharedFromThis(this));
}

void Hero_Pumpkin::Draw() {
	Hero::Draw();
	weapon->Draw();
}

bool Hero_Pumpkin::Update() {
	if (mainLogic.Resume()) return true;
	if (weapon) {
		weapon->mainLogic();
	}
	return false;
}

xx::Task<> Hero_Pumpkin::MainLogic_() {
	while (true) {

		// keyboard move control
		if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
			pos += r->second * speed;
			if (flipX && ((int)r->first & (int)MoveDirections::Right)) {
				flipX = false;
			} else if ((int)r->first & (int)MoveDirections::Left) {
				flipX = true;
			}
			ForwardFrame(cFrameInc * (speed / cSpeed), cFrameMaxIndex);
		} else {
			idle();
		}
		weaponPos = pos + cHookOffset;

		// eat experiences
		ForeachByRange<Experience>(gLooper.experiencesGrid, gLooper.sgrdd, pos, eatExperienceDistance, [&](Experience* m) {
			m->FlyTo(this);
		});

		co_yield 0;
	}
}
