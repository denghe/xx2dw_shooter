#pragma once
#include "game_sprite.h"

template<typename Owner>
struct HandWeapon : Sprite {
	constexpr static XY cAnchor{ 0.f, 0.5f };
	constexpr static float cFrameMaxChangeRadians{ M_PI * 10 / gDesign.fps };

	xx::Weak<Owner> owner;

	void Init(xx::Weak<Owner> owner_) {
		mainLogic = MainLogic();
		//radius = cRadius;
		pos = owner_->pos;
		owner = std::move(owner_);
		frames = &gLooper.frames_weapon;
		frameIndex = 0;
		body.SetAnchor(cAnchor);
	}

	xx::Task<> MainLogic() {
		while (owner) {

			pos = owner->pos + Owner::cHookOffset;
			auto v = gLooper.mousePos - gLooper.camera.ToGLPos(pos);
			auto r = -std::atan2(v.y, v.x);
			StepRadians(r, cFrameMaxChangeRadians);

			co_yield 0;
		}
	}

	// todo: Fire
};
