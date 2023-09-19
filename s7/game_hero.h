#pragma once
#include "game_hand_weapon.h"

struct Hero : Sprite {
	constexpr static XY cAnchor{ 0.5f, 0.f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };
	constexpr static XY cHookOffset{ 0, -5.f };

	float frameIndexFrom{}, frameIndexTo{};
	float speed{ cSpeed };

	xx::Weak<HandWeapon<Hero>> weapon;

	void Init(XY const& bornPos) {
		mainLogic = MainLogic();
		radius = cRadius;
		pos = bornPos;
		frames = &gLooper.frames_pumpkin;
		body.SetAnchor(cAnchor);
		weapon = gLooper.heroHandWeapons.Emplace().Emplace();
		weapon->Init(xx::WeakFromThis(this));
	}

	xx::Task<> MainLogic() {
		while (true) {

			// keyboard move control
			if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
				pos += r->second * speed;
				if (flipX && ((int)r->first & (int)MoveDirections::Right)) {
					flipX = false;
				} else if ((int)r->first & (int)MoveDirections::Left) {
					flipX = true;
				}
				ForwardFrame(cFrameInc * speed, cFrameMaxIndex);
			} else {
				Idle();
			}

			co_yield 0;
		}
	}

};
