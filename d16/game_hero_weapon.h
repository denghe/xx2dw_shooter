#pragma once
#include <game_hero.h>

namespace Hero {


	struct Weapon : Sprite {
		//xx::Weak<WeaponConfig>
		xx::Weak<Character> owner;
		float damage{};
		float nextFireSecs{};
		// todo: buff ?
	};

}
