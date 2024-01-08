#pragma once
#include <game_looper.h>

namespace Config {

	enum class WeaponTypes {
		Unknown,
		Particle,		// fly instance
		Wave,			// close combat shape? explosion? sound? ( fan diffusion attack )
		Lightning,		// teleport electronic explosion ( teleport range attack )
		Laser,			// energy / high brightness light ( line attack )
		Burn,			// fire dot attack
		Curse,			// debuff attack
		Summon			// summon friendly forces or resurrect corpses?
	};

	// 3 types: emitter, transfer, effector

	struct WeaponConfig {
		WeaponTypes cType{};
		float cDamage{};
		float cCastDelaySeconds{};
	};

}
