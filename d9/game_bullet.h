#pragma once
#include "game_weapon.h"

struct Bullet : Sprite {
	xx::Weak<Player> player;
	int32_t damage;
	// todo: hit check ?
};

struct Bullet_EyeFire : Bullet {
	constexpr static char const* cResPrefix{ "eye_fire_" };
	constexpr static XY cAnchor{ 0.85f, 0.5f };
	constexpr static float cRadius{ 5.f };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cLifeDelta{ 1.f / 5.f / gDesign.fps };
	constexpr static float cSpeed{ 300.f / gDesign.fps };
	constexpr static int32_t cDamage{ 10 };

	XY inc{};
	float speed{ cSpeed };

	void Init(Weapon* hw, XY const& pos_, float r, float c, float s);
	xx::Task<> MainLogic_();
};

struct Bullet_EyeFireBlue : Bullet {
	constexpr static char const* cResPrefix{ "eye_fire_" };	// known issue: no picture
	constexpr static XY cAnchor{ 0.85f, 0.5f };
	constexpr static float cRadius{ 5.f };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cLifeDelta{ 1.f / 5.f / gDesign.fps };
	constexpr static float cSpeed{ 300.f / gDesign.fps };
	constexpr static int32_t cDamage{ 8 };

	XY inc{};
	float speed{ cSpeed };

	void Init(Weapon* hw, XY const& pos_, float r, float c, float s);
	xx::Task<> MainLogic_();
};

struct Bullet_Fireball : Bullet {
	constexpr static char const* cResPrefix{ "fireball_white_" };
	constexpr static XY cAnchor{ 0.85f, 0.5f };
	constexpr static float cRadius{ 4.f };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cLifeDelta{ 1.f / 5.f / gDesign.fps };
	constexpr static float cSpeed{ 300.f / gDesign.fps };
	constexpr static int32_t cDamageMin{ 15 }, cDamageMax{ 30 };

	XY inc{};
	float speed{ cSpeed };

	void Init(Weapon* hw, XY const& pos_, float r, float c, float s);
	xx::Task<> MainLogic_();
};

struct Bullet_Explosion : Bullet {
	constexpr static char const* cResPrefix{ "explosion_1_" };
	constexpr static XY cAnchor{ 0.5f, 0.5f };
	constexpr static float cScale{ 2.f };
	constexpr static float cRadius{ 16.f * cScale };
	constexpr static float cDamageFrameIndex{ 2 };
	constexpr static float cFrameInc{ 20.f / gDesign.fps };

	void Init(Bullet* fb);
	xx::Task<> MainLogic_();
};
