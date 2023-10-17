#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct Sprite;
struct Hero;
template<typename Owner>
struct MagicWeapon;
struct Afterimage;

struct YSprite {
	float y;
	Sprite const* s;
};

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
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
	xx::ListLink<xx::Shared<Afterimage>, int32_t> afterimages;
	xx::List<YSprite, int32_t> ysprites;

	Camera camera;
};
extern GameLooper gLooper;
