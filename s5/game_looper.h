#pragma once
#include "game_looper_base.h"

struct Hero;
template<typename Owner>
struct MagicWeapon;

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
	xx::List<xx::Shared<Hero>, int32_t> heros;
	xx::List<xx::Shared<MagicWeapon<Hero>>, int32_t> heroMagicWeapons;

	Camera camera;
};
extern GameLooper gLooper;
