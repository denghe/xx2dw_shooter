#pragma once
#include "game_looper_base.h"

constexpr int32_t gGridCellDiameter = 16, gGridNumCols = 256, gGridNumRows = 256;
constexpr XY gGridBasePos{ gGridCellDiameter * gGridNumCols / 2, gGridCellDiameter * gGridNumRows / 2 };

struct Player;
struct DamageNumber;
struct Sprite;
struct Blood;
struct Afterimage;
struct Hero;
struct Weapon;
struct Bullet;
struct Monster;

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
	std::vector<xx::Shared<Frame>> frames_number_outlined;
	std::vector<xx::Shared<Frame>> frames_blood_4;
	std::vector<xx::Shared<Frame>> frames_eye_fire;
	std::vector<xx::Shared<Frame>> frames_eye_fire_blue;
	std::vector<xx::Shared<Frame>> frames_fireball_10;
	std::vector<xx::Shared<Frame>> frames_dragon_babywhite;

	// players
	xx::Shared<Player> player1;

	// objs
	xx::ListLink<xx::Shared<Hero>, int32_t> heros;
	xx::ListLink<xx::Shared<Bullet>, int32_t> bullets;
	xx::ListLink<xx::Shared<Afterimage>, int32_t> afterimages;
	xx::ListLink<xx::Shared<DamageNumber>, int32_t> damageNumbers;
	xx::ListLink<xx::Shared<Blood>, int32_t> bloods;

	xx::ListDoubleLink<xx::Shared<Monster>, int32_t, uint32_t> monsters;
	SpaceGridC<Monster, XY> monstersGrid;
	SpaceGridRingDiffuseData sgrdd;

	Camera camera;
};
extern GameLooper gLooper;
