#pragma once
#include "game_looper_base.h"
struct Player;
struct Sprite;
struct Afterimage;
struct Hero;
struct HandWeapon;
struct Bullet;

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
	std::vector<xx::Shared<Frame>> frames_eye_fire;

	// players
	xx::Shared<Player> player1;

	// sprites
	xx::ListLink<xx::Shared<Hero>, int32_t> heros;
	xx::ListLink<xx::Shared<Bullet>, int32_t> bullets;
	xx::ListLink<xx::Shared<Afterimage>, int32_t> afterimages;

	Camera camera;
};
extern GameLooper gLooper;
