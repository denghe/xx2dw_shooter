#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 240> gDesign;

constexpr int32_t gGridCellDiameter = 16, gGridNumCols = 256, gGridNumRows = 256;

struct Player;
struct DamageNumber;
struct Sprite;
struct Blood;
struct Afterimage;
struct Hero;
struct Weapon;
struct Bullet;
struct Monster;
struct Experience;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<xx::Ref<Frame>> frames_pumpkin;
	std::vector<xx::Ref<Frame>> frames_weapon;
	std::vector<xx::Ref<Frame>> frames_number_outlined;
	std::vector<xx::Ref<Frame>> frames_blood_4;
	std::vector<xx::Ref<Frame>> frames_icon_gem;
	std::vector<xx::Ref<Frame>> frames_eye_fire;
	std::vector<xx::Ref<Frame>> frames_eye_fire_blue;
	std::vector<xx::Ref<Frame>> frames_fireball_10;
	std::vector<xx::Ref<Frame>> frames_explosion;
	std::vector<xx::Ref<Frame>> frames_dragon_babywhite;

	// players
	xx::Shared<Player> player1;

	// objs
	xx::ListLink<xx::Shared<Hero>, int32_t> heros;
	xx::ListLink<xx::Shared<Bullet>, int32_t> bullets;
	xx::ListLink<xx::Shared<Afterimage>, int32_t> afterimages;
	xx::ListLink<xx::Shared<DamageNumber>, int32_t> damageNumbers;
	xx::ListLink<xx::Shared<Blood>, int32_t> bloods;
	xx::ListLink<xx::Shared<Experience>, int32_t> flyingExperiences;

	xx::ListDoubleLink<xx::Shared<Experience>, int32_t, uint32_t> experiences;
	SpaceGridC<Experience, XY> experiencesGrid;

	xx::ListDoubleLink<xx::Shared<Monster>, int32_t, uint32_t> monsters;
	SpaceGridC<Monster, XY> monstersGrid;

	SpaceGridRingDiffuseData sgrdd;

	Camera camera;
};
extern GameLooper gLooper;
