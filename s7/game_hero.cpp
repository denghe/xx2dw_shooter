#include "pch.h"

void Hero_Pumpkin::Init(xx::Shared<Player> const& player_, XY const& bornPos) {
	mainLogic = MainLogic();
	idle = Idle_ScaleY();
	player = player_;
	radius = cRadius;
	pos = bornPos;
	speed = cSpeed;
	frames = &gLooper.frames_pumpkin;
	body.SetAnchor(cAnchor);
	weapon.Emplace<Weapon_Sword1>()->Init(xx::SharedFromThis(this));
}

void Hero_Pumpkin::Draw() const {
	Hero::Draw();
	weapon->Draw();
}

xx::Task<> Hero_Pumpkin::MainLogic() {
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

		co_yield 0;
	}
}
