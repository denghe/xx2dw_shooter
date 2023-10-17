#pragma once
#include "game_drawable_sprite.h"

struct Weapon : Sprite {
	constexpr static char const* cResPrefix{ "weapon_" };
	xx::Weak<Player> player;
	xx::Weak<Hero> hero;
	float nextFireSecs{};
};

struct Weapon_1_Sword : Weapon {
	constexpr static XY cAnchor{ 0.1f, 0.5f };
	constexpr static float cFrameIndex{ 0 };
	constexpr static float cFrameMaxChangeRadians{ float(M_PI * 10 / gDesign.fps) };
	constexpr static float cFireDelaySecs{ 0.05f };
	constexpr static float cFireDistance{ 27 };
	constexpr static float cFireRecoilDelaySecs{ 0.02f };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };
	constexpr static float cSearchDistance{ 250.f };

	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic_();
	xx::Task<> MainLogic2();
};
