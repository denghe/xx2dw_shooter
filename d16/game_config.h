#pragma once
#include <game_looper.h>

namespace Config {

	enum class WeaponTypes {
		Unknown,
		Particle,		// fly instance
		Shape,			// close combat
		Wave,			// explosion? sound? ( fan diffusion attack )
		Lightning,		// teleport electronic explosion ( teleport range attack )
		Laser,			// high brightness light ( line attack )
		Burn,			// fire dot attack
		Curse,			// debuff attack
		Summon			// summon friendly forces or resurrect corpses?
	};

	struct WeaponConfig {
		WeaponTypes cType{};
		float cDamage{};
		float cCastDelaySeconds{};
	};

}
