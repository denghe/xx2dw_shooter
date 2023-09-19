#pragma once
#include "game_looper_base.h"

struct Hero;
template<typename Owner>
struct HandWeapon;
struct Afterimage;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Shared<Frame>> frames_pumpkin;
	std::vector<xx::Shared<Frame>> frames_weapon;

	// sprites
	xx::ListLink<xx::Shared<Hero>, int32_t> heros;
	xx::ListLink<xx::Shared<HandWeapon<Hero>>, int32_t> heroHandWeapons;
	xx::ListLink<xx::Shared<Afterimage>, int32_t> afterimages;

	Camera camera;
};
extern GameLooper gLooper;
