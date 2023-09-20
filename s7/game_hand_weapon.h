﻿#pragma once
#include "game_sprite.h"

struct HandWeapon : Sprite {
	xx::Weak<Player> player;
	xx::Weak<Hero> hero;
	XY firePos{};
	float nextFireSecs{};
};

struct HandWeapon_Sword1 : HandWeapon {
	constexpr static XY cAnchor{ 0.1, 0.5 };
	constexpr static float cFrameIndex{ 0 };
	constexpr static float cFrameMaxChangeRadians{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDelaySecs{ 0.02 };
	constexpr static float cFireDistance{ 27 };

	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic();
};
