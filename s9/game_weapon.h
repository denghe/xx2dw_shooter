#pragma once
#include "game_drawable_sprite.h"

struct Weapon : Sprite {
	constexpr static char const* cResPrefix{ "weapon_" };
	xx::Weak<Player> player;
	xx::Weak<Hero> hero;
	float nextFireSecs{};
};

struct Weapon_1_Sword : Weapon {
	constexpr static XY cAnchor{ 0, 0.5 };
	constexpr static float cFrameIndex{ 0 };
	constexpr static float cFrameMaxChangeRadians{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDelaySecs{ 0.05 };
	constexpr static float cFireDistance{ 27 };
	constexpr static float cFireRecoilDelaySecs{ 0.02 };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };
	constexpr static float cSearchDistance{ 50.f };
	constexpr static float cDamage{ 30.f };
	constexpr static std::array<PosRadius, 6> cCircles{ PosRadius{{26,0},4}, {{22,0},4}, {{18,0},4}, {{14,0},4}, {{10,0},4}, {{6,0},4} };

	std::array<PosRadius, 6> circles{ cCircles };
	void RotateCirclesAndHitCheck();

	virtual void Draw() override;
	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic();
};

struct Weapon_19_Stuff : Weapon {
	constexpr static XY cAnchor{ 0.5, 0.5 };
	constexpr static float cFrameIndex{ 18 };
	constexpr static float cFrameMaxChangeRadians{ M_PI * 10 / gDesign.fps };
	constexpr static float cFireDelaySecs{ 0.1 };
	constexpr static float cFireDistance{ 25 };
	constexpr static float cFireRecoilDelaySecs{ 0.02 };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };
	constexpr static float cSearchDistance{ 200.f };
	constexpr static float cDamage{ 30.f };

	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic();
};
