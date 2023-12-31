﻿#pragma once
#include "game_drawable_sprite.h"
#include "game_damage_number.h"
#include "game_blood.h"
#include "game_experience.h"

struct Monster : Sprite, Removeable<Monster>, SpaceGridCItem<Monster, XY> {
	constexpr static float cDamageEffectDurationSecs{ 0.2f };

	int32_t hp{};
	float speed{};
	float damageEffectLeftDuration{};
	xx::TaskGuard damageEffect;

	~Monster() {
		SGCRemove();
	}

	void Hit(int damage, Hero* hiter = nullptr) {
		assert(damage > 0);
		if (damage >= hp) {
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, hiter ? pos - hiter->pos : XY{}, { 255,0,0,255 }, hp);
			gLooper.bloods.Emplace().Emplace()->Init(pos);
			Experience::CreateTo(gLooper.experiences)->Init(pos, 0, 10);
			RemoveFromOwner();
		} else {
			hp -= damage;
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, hiter ? pos - hiter->pos : XY{}, { 255,255,255,255 }, damage);
			if (damageEffectLeftDuration <= 0) {
				damageEffect(gEngine->tasks, [this]()->xx::Task<> {
					while (damageEffectLeftDuration > 0) {
						damageEffectLeftDuration -= gDesign.frameDelay;
						body.SetColorplus(1);
						co_yield 0;
					}
					body.SetColorplus(0);
				});
			}
			damageEffectLeftDuration = cDamageEffectDurationSecs;	// renew
		}
	}

};

struct Monster_Dragon_BabyWhite : Monster {
	constexpr static char const* cResPrefix{ "dragon_babywhite_" };
	constexpr static XY cAnchor{ 0.5f, 0.3f };
	constexpr static float cScale{ 0.2f };
	constexpr static float cRadius{ 5 * cScale };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	void Init(int hp_, XY const& pos_);
	xx::Task<> MainLogic_();
};
