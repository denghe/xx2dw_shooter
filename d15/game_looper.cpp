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

		// following code is copy from d20.txt
		tp->GetToByPrefix(frames_axe            , "axe_"            );
		tp->GetToByPrefix(frames_blood_4        , "blood_4_"        );
		tp->GetToByPrefix(frames_border_1       , "border_1_"       );
		tp->GetToByPrefix(frames_border_2       , "border_2_"       );
		tp->GetToByPrefix(frames_border_3       , "border_3_"       );
		tp->GetToByPrefix(frames_border_4       , "border_4_"       );
		tp->GetToByPrefix(frames_bullet         , "bullet_"         );
		tp->GetToByPrefix(frames_cheses_1       , "cheses_1_"       );
		tp->GetToByPrefix(frames_cheses_2       , "cheses_2_"       );
		tp->GetToByPrefix(frames_coin_1         , "coin_1_"         );
		tp->GetToByPrefix(frames_coin_2         , "coin_2_"         );
		tp->GetToByPrefix(frames_coin_3         , "coin_3_"         );
		tp->GetToByPrefix(frames_creature_1     , "creature_1_"     );
		tp->GetToByPrefix(frames_crucible_4     , "crucible_4_"     );
		tp->GetToByPrefix(frames_dagger         , "dagger_"         );
		tp->GetToByPrefix(frames_dot_1          , "dot_1_"          );
		tp->GetToByPrefix(frames_explosion_12   , "explosion_12_"   );
		tp->GetToByPrefix(frames_explosion_1    , "explosion_1_"    );
		tp->GetToByPrefix(frames_explosion_9    , "explosion_9_"    );
		tp->GetToByPrefix(frames_eye_fire       , "eye_fire_"       );
		tp->GetToByPrefix(frames_firearrow      , "firearrow_"      );
		tp->GetToByPrefix(frames_fireball_white , "fireball_white_" );
		tp->GetToByPrefix(frames_floor          , "floor_"          );
		tp->GetToByPrefix(frames_gem_1          , "gem_1_"          );
		tp->GetToByPrefix(frames_gem_2          , "gem_2_"          );
		tp->GetToByPrefix(frames_grass          , "grass_"          );
		tp->GetToByPrefix(frames_hammer         , "hammer_"         );
		tp->GetToByPrefix(frames_human_1        , "human_1_"        );
		tp->GetToByPrefix(frames_human_2        , "human_2_"        );
		tp->GetToByPrefix(frames_human_3        , "human_3_"        );
		tp->GetToByPrefix(frames_human_4        , "human_4_"        );
		tp->GetToByPrefix(frames_icon_book_1    , "icon_book_1_"    );
		tp->GetToByPrefix(frames_knife          , "knife_"          );
		tp->GetToByPrefix(frames_laser          , "laser_"          );
		tp->GetToByPrefix(frames_lightning_2    , "lightning_2_"    );
		tp->GetToByPrefix(frames_mine           , "mine_"           );
		tp->GetToByPrefix(frames_monster_1      , "monster_1_"      );
		tp->GetToByPrefix(frames_monster_2      , "monster_2_"      );
		tp->GetToByPrefix(frames_monster_3      , "monster_3_"      );
		tp->GetToByPrefix(frames_no             , "no_"             );
		tp->GetToByPrefix(frames_number_drop    , "number_drop_"    );
		tp->GetToByPrefix(frames_number_outlined, "number_outlined_");
		tp->GetToByPrefix(frames_pointer        , "pointer_"        );
		tp->GetToByPrefix(frames_potion_4       , "potion_4_"       );
		tp->GetToByPrefix(frames_staff          , "staff_"          );
		tp->GetToByPrefix(frames_sword          , "sword_"          );
		tp->GetToByPrefix(frames_symbol_1       , "symbol_1_"       );
		tp->GetToByPrefix(frames_symbol_2       , "symbol_2_"       );
		tp->GetToByPrefix(frames_wall           , "wall_"           );
		tp->GetToByPrefix(frames_yes            , "yes_"            );

		frame_border_1_2322 = frames_border_1[0];
		frame_border_2_4522 = frames_border_2[0];
		frame_border_3_2222 = frames_border_3[0];
		frame_border_4_1122 = frames_border_4[0];
		frame_dot_1_22 = frames_dot_1[0];
		frame_no = frames_no[0];
		frame_yes = frames_yes[0];
		// ...
	}

	s9cfg_btn.frame = frame_border_1_2322;
	s9cfg_btn.texScale = { 2, 2 };
	s9cfg_btn.center = { 2, 3, 2, 2 };
	s9cfg_btn.color = { 0x5f, 0x15, 0xd9, 0xff };

	s9cfg_panel.frame = frame_border_2_4522;
	s9cfg_panel.texScale = { 2, 2 };
	s9cfg_panel.center = { 4, 5, 2, 2 };
	s9cfg_panel.color = RGBA8_White;

	s9cfg_hp.frame = frame_border_4_1122;
	s9cfg_hp.texScale = { 3, 1 };
	s9cfg_hp.center = { 1, 1, 2, 2 };
	s9cfg_hp.color = {90,90,90,255};

	//co_await AsyncSwitchTo<SceneMainMenu>();
	co_await AsyncSwitchTo<SceneTest1>();
}
