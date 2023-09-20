#pragma once
#include "game_hand_weapon.h"

struct Hero : Sprite {
	float frameIndexFrom{}, frameIndexTo{};
	float speed{};
	XY weaponPos{};
	xx::Shared<HandWeapon> weapon;
	xx::Weak<Player> player;
};

struct Hero_Pumpkin : Hero {
	constexpr static XY cAnchor{ 0.5f, 0.f };
	constexpr static float cRadius{ 6.f };
	constexpr static float cFrameMaxIndex{ 4.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };
	constexpr static XY cHookOffset{ 0, -5.f };

	void Init(xx::Shared<Player> const& player_, XY const& bornPos);
	void Draw() const;
	xx::Task<> MainLogic();
};
