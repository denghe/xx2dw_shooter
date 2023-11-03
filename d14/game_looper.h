#pragma once
#include <engine.h>

constexpr GDesign<1280, 800, 60> gDesign;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	xx::Task<> MainTask();

	// res

	// following code is copy from d20.txt
	xx::List<xx::Ref<Frame>, int32_t>
		frames_axe,
		frames_blood_4,
		frames_border_1,
		frames_border_2,
		frames_border_3,
		frames_border_4,
		frames_bullet,
		frames_cheses_1,
		frames_cheses_2,
		frames_coin_1,
		frames_coin_2,
		frames_coin_3,
		frames_creature_1,
		frames_crucible_4,
		frames_dagger,
		frames_dot_1,
		frames_explosion_12,
		frames_explosion_1,
		frames_explosion_9,
		frames_eye_fire,
		frames_firearrow,
		frames_fireball_white,
		frames_floor,
		frames_gem_1,
		frames_gem_2,
		frames_grass,
		frames_hammer,
		frames_human_1,
		frames_human_2,
		frames_human_3,
		frames_human_4,
		frames_icon_book_1,
		frames_knife,
		frames_laser,
		frames_lightning_2,
		frames_mine,
		frames_monster_1,
		frames_monster_2,
		frames_monster_3,
		frames_no,
		frames_number_drop,
		frames_number_outlined,
		frames_pointer,
		frames_potion_4,
		frames_staff,
		frames_sword,
		frames_symbol_1,
		frames_symbol_2,
		frames_wall,
		frames_yes;

	xx::Ref<Frame> frame_border_1_2322
		, frame_border_2_4522
		, frame_border_3_2222
		, frame_border_4_1122
		, frame_dot_1_22
		, frame_no, frame_yes
		;
	// ...

	// cfgs
	Scale9SpriteConfig s9cfg_btn, s9cfg_panel, s9cfg_hp;

};

extern GameLooper gLooper;
