#pragma once
#include "game_drawable_sprite.h"
#include "game_damage_number.h"
#include "game_blood.h"
#include "game_experience.h"

struct Monster : Sprite, Removeable<Monster>, SpaceGridCItem<Monster, XY> {
	constexpr static float cDamageEffectDurationSecs{ 0.2 };

	int32_t hp{};
	float speed{};
	float damageEffectLeftDuration{};
	xx::TaskDeleter damageEffect;

	~Monster() {
		SGCRemove();
	}

	void Hit(int damage) {
		assert(damage > 0);
		if (damage >= hp) {
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, { 255,0,0,255 }, hp);
			gLooper.bloods.Emplace().Emplace()->Init(pos);
			Experience::CreateTo(gLooper.experiences)->Init(pos, 0, 10);
			RemoveFromOwner();
		} else {
			hp -= damage;
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, { 255,255,255,255 }, damage);
			if (damageEffectLeftDuration <= 0) {
				damageEffect(gLooper.tasks, [this]()->xx::Task<> {
					while (damageEffectLeftDuration > 0) {
						damageEffectLeftDuration -= gDesign.frameDelay;
						body.SetColormulti(255);
						co_yield 0;
					}
					body.SetColormulti(1);
				});
			}
			damageEffectLeftDuration = cDamageEffectDurationSecs;	// renew
		}
	}

};

struct Monster_Dragon_BabyWhite : Monster {
	constexpr static char const* cResPrefix{ "dragon_babywhite_" };
	constexpr static XY cAnchor{ 0.5, 0.3 };
	constexpr static float cScale{ 0.2 };
	constexpr static float cRadius{ 5 * cScale };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	void Init(int hp_, XY const& pos_);
	xx::Task<> MainLogic_();
};
