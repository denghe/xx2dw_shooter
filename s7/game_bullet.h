#pragma once
#include "game_hand_weapon.h"

struct Bullet : Sprite {
	xx::Weak<Player> player;
	// todo: hit check ?
};

struct Bullet_EyeFire : Bullet {
	constexpr static XY cAnchor{ 0.85f, 0.5f };
	constexpr static float cRadius{ 5.f };
	constexpr static float cFrameMaxIndex{ 7.f };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cLifeDelta{ 1.f / 5.f / gDesign.fps };
	constexpr static float cSpeed{ 300.f / gDesign.fps };

	XY inc{};

	void Init(HandWeapon* hw, XY const& pos_, float r, float c, float s);
	xx::Task<> MainLogic();
};
