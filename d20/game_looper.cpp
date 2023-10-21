#include <pch.h>
#include <all.h>

xx::Task<> GameLooper::MainTask() {
	{
#ifdef __EMSCRIPTEN__
		auto tp = co_await AsyncLoadTexturePackerFromUrl
#else
		auto tp = LoadTexturePacker
#endif
		("res/d20.blist");
		tp->GetToByPrefix(frames_axe, "axe_");
		tp->GetToByPrefix(frames_blood_4, "blood_4_");
		tp->GetToByPrefix(frames_bullet, "bullet_");
		frame_border_1_2322 = tp->Get("border_1_2322");
		frame_border_2_4522 = tp->Get("border_2_4522");
		tp->GetToByPrefix(frames_cheses_1, "cheses_1_");
		tp->GetToByPrefix(frames_cheses_2, "cheses_2_");
		tp->GetToByPrefix(frames_coin_1, "coin_1_");
		tp->GetToByPrefix(frames_coin_2, "coin_2_");
		tp->GetToByPrefix(frames_coin_3, "coin_3_");
		tp->GetToByPrefix(frames_creature_1, "creature_1_");
		tp->GetToByPrefix(frames_crucible_4, "crucible_4_");
		tp->GetToByPrefix(frames_dagger, "dagger_");
		frame_dot_1_22 = tp->Get("dot_1_22");
		tp->GetToByPrefix(frames_explosion_1, "explosion_1_");
		tp->GetToByPrefix(frames_explosion_9, "explosion_9_");
		tp->GetToByPrefix(frames_explosion_12, "explosion_12_");
		tp->GetToByPrefix(frames_eye_fire, "eye_fire_");
		tp->GetToByPrefix(frames_firearrow, "firearrow_");
		tp->GetToByPrefix(frames_fireball_white, "fireball_white_");
		tp->GetToByPrefix(frames_floor, "floor_");
		tp->GetToByPrefix(frames_gem_1, "gem_1_");
		tp->GetToByPrefix(frames_gem_2, "gem_2_");
		tp->GetToByPrefix(frames_grass, "grass_");
		tp->GetToByPrefix(frames_hammer, "hammer_");
		tp->GetToByPrefix(frames_human_1, "human_1_");
		tp->GetToByPrefix(frames_human_2, "human_2_");
		tp->GetToByPrefix(frames_human_3, "human_3_");
		tp->GetToByPrefix(frames_human_4, "human_4_");
		tp->GetToByPrefix(frames_icon_book_1, "icon_book_1_");
		tp->GetToByPrefix(frames_knife, "knife_");
		tp->GetToByPrefix(frames_laser, "laser_");
		tp->GetToByPrefix(frames_lightning_2, "lightning_2_");
		tp->GetToByPrefix(frames_mine, "mine_");
		tp->GetToByPrefix(frames_monster_1, "monster_1_");
		tp->GetToByPrefix(frames_monster_2, "monster_2_");
		tp->GetToByPrefix(frames_monster_3, "monster_3_");
		frame_no = tp->Get("no_1");
		tp->GetToByPrefix(frames_number_drop, "number_drop_");
		tp->GetToByPrefix(frames_number_outlined, "number_outlined_");
		tp->GetToByPrefix(frames_pointer, "pointer_");
		tp->GetToByPrefix(frames_potion_4, "potion_4_");
		tp->GetToByPrefix(frames_staff, "staff_");
		tp->GetToByPrefix(frames_sword, "sword_");
		tp->GetToByPrefix(frames_symbol_1, "symbol_1_");
		tp->GetToByPrefix(frames_wall, "wall_");
		frame_yes = tp->Get("yes_1");
		// ...
	}

	co_await AsyncSwitchTo<SceneMainMenu>();
}
