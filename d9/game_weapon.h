#pragma once
#include "game_drawable_sprite.h"

struct Weapon : Sprite {
	constexpr static char const* cResPrefix{ "sword_" };
	xx::Weak<Player> player;
	xx::Weak<Hero> hero;
	float nextFireSecs{};
};

struct Weapon_1_Sword : Weapon {
	constexpr static XY cAnchor{ 0.f, 0.5f };
	constexpr static float cFrameIndex{ 0.f };
	constexpr static float cFrameMaxChangeRadians{ float(M_PI * 10 / gDesign.fps) };
	constexpr static float cFireDelaySecs{ 0.05f };
	constexpr static float cFireDistance{ 27.f };
	constexpr static float cFireRecoilDelaySecs{ 0.02f };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };
	constexpr static float cSearchDistance{ 50.f };
	constexpr static int32_t cDamage{ 30 };
	constexpr static std::array<PosRadius, 6> cCircles{ PosRadius{{26,0},4}, {{22,0},4}, {{18,0},4}, {{14,0},4}, {{10,0},4}, {{6,0},4} };

	std::array<PosRadius, 6> circles{ cCircles };
	void RotateCirclesAndHitCheck();

	virtual void Draw() override;
	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic();
};

struct Weapon_19_Stuff : Weapon {
	constexpr static XY cAnchor{ 0.5f, 0.5f };
	constexpr static float cFrameIndex{ 18.f };
	constexpr static float cFrameMaxChangeRadians{ float(M_PI * 10 / gDesign.fps) };
	constexpr static float cFireDelaySecs{ 0.1f };
	constexpr static float cFireDistance{ 25.f };
	constexpr static float cFireRecoilDelaySecs{ 0.02f };
	constexpr static float cFireRecoilSpeed{ 400.f / gDesign.fps };
	constexpr static float cSearchDistance{ 200.f };
	constexpr static int32_t cDamage{ 30 };

	void Init(xx::Shared<Hero> const& hero_);
	xx::Task<> MainLogic();
};
