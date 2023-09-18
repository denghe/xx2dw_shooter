#pragma once
#include "game_looper_base.h"

struct Sprite;
struct Hero;
template<typename Owner>
struct MagicWeapon;
struct MagicWeaponShadow;

struct YSprite {
	float y;
	Sprite const* s;
};

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<std::vector<xx::Shared<Frame>>> frames_heros;
	std::vector<xx::Shared<Frame>> frames_magicWeapon;

	// sprites
	xx::ListLink<xx::Shared<Hero>, int32_t> heros;
	xx::ListLink<xx::Shared<MagicWeapon<Hero>>, int32_t> heroMagicWeapons;
	xx::ListLink<xx::Shared<MagicWeaponShadow>, int32_t> heroMagicWeaponShadows;
	xx::List<YSprite, int32_t> ysprites;

	Camera camera;
};
extern GameLooper gLooper;
