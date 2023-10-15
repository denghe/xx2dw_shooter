#pragma once
#include "game_weapon.h"

struct Hero : Sprite {
	float frameIndexFrom{}, frameIndexTo{};
	float speed{};
	float eatExperienceDistance{};
	XY weaponPos{};
	xx::Shared<Weapon> weapon;
	xx::Weak<Player> player;
};

struct Hero_Pumpkin : Hero {
	constexpr static char const* cResPrefix{ "pumpkin_" };
	constexpr static XY cAnchor{ 0.5f, 0.f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 60.f / gDesign.fps };
	constexpr static float cEatExperienceDistance{ 40 };
	constexpr static XY cHookOffset{ 0, -5.f };

	void Init(xx::Shared<Player> const& player_, XY const& bornPos);
	bool Update();
	virtual void Draw() override;
	xx::Task<> MainLogic_();
};

struct Hero_FloatingEye : Hero {
	constexpr static char const* cResPrefix{ "floating_eye_" };
	constexpr static XY cAnchor{ 0.5f, 0.f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 60.f / gDesign.fps };
	constexpr static float cEatExperienceDistance{ 140 };
	constexpr static XY cHookOffset{ 0, -10.f };

	void Init(xx::Shared<Player> const& player_, XY const& bornPos);
	bool Update();
	virtual void Draw() override;
	xx::Task<> MainLogic_();
};
