#pragma once
#include "game_weapon.h"

struct Bullet : Sprite {
	xx::Weak<Player> player;
	// todo: hit check ?
};

struct Bullet_EyeFire : Bullet {
	using ThisType = Bullet_EyeFire;
	constexpr static char const* cResPrefix{ "eye_fire_" };
	constexpr static XY cAnchor{ 0.85f, 0.5f };
	constexpr static float cRadius{ 5.f };
	constexpr static float cFrameMaxIndex{ 7.f };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cLifeDelta{ 1.f / 5.f / gDesign.fps };
	constexpr static float cSpeed{ 300.f / gDesign.fps };
	constexpr static float cDamage{ 10 };

	XY inc{};
	float speed{ cSpeed };
	float damage{ cDamage };

	void Init(Weapon* hw, XY const& pos_, float r, float c, float s);
	xx::Task<> MainLogic{ MainLogic_() };
	xx::Task<> MainLogic_();
};
