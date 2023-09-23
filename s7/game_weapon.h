#pragma once
#include "game_drawable_sprite.h"

struct Weapon : Sprite {
	constexpr static char const* cResPrefix{ "weapon_" };
	xx::Weak<Player> player;
	xx::Weak<Hero> hero;
	float nextFireSecs{};
};

struct Weapon_Sword1 : Weapon {
	using ThisType = Weapon_Sword1;
	constexpr static XY cAnchor{ 0.1, 0.5 };
	constexpr static float cFrameIndex{ 0 };
	constexpr static float cFrameMaxChangeRadians{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDelaySecs{ 0.1 };
	constexpr static float cFireDistance{ 27 };
	constexpr static float cFireRecoilDelaySecs{ 0.02 };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };

	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic{ MainLogic_() };
	xx::Task<> MainLogic_();
};
