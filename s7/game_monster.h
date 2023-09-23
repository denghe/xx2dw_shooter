#pragma once
#include "game_drawable_sprite.h"
#include "game_damage_number.h"
#include "game_blood.h"
#include "game_experience.h"

struct Monster : Sprite, SpaceGridCItem<Monster, XY> {
	constexpr static float cDamageEffectDurationSecs{ 0.2 };

	int32_t hp{};
	float speed{};
	float damageEffectLeftDuration{};
	xx::TaskDeleter damageEffect;

	xx::ListDoubleLink<xx::Shared<Monster>, int32_t, uint32_t>* container{};
	xx::ListDoubleLinkIndexAndVersion<int32_t, uint32_t> indexAndVersionByContainer;

	template<typename MonsterDerived>
	static xx::Shared<MonsterDerived> CreateTo(xx::ListDoubleLink<xx::Shared<Monster>, int32_t, uint32_t>& container) {
		auto rtv = xx::Make<MonsterDerived>();
		container.Emplace(rtv);
		rtv->container = &container;
		rtv->indexAndVersionByContainer = container.Tail();
		return rtv;
	}

	void RemoveFromOwner() {
		auto c = container;
		xx_assert(c);
		auto iv = indexAndVersionByContainer;
		assert(iv.index != -1);
		container = {};
		indexAndVersionByContainer = { -1, 0 };
		c->Remove(iv);
	}

	~Monster() {
		SGCRemove();
	}

	void Hit(int damage) {
		assert(damage > 0);
		gLooper.bloods.Emplace().Emplace()->Init(pos);
		if (damage >= hp) {
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, { 255,0,0,255 }, hp);
			gLooper.experiences.Emplace().Emplace()->Init(pos, 0, 10);
			RemoveFromOwner();
		} else {
			hp -= damage;
			gLooper.damageNumbers.Emplace().Emplace()->Init(pos, { 255,255,255,255 }, damage);	// todo: calc pos ??
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

	inline static Monster* FindNeighbor(SpaceGridC<Monster, XY>& container, XY const& pos, float radius) {
		assert(radius < gGridCellDiameter);
		auto crIdx = container.PosToCrIdx(pos);
		Monster* r{};
		container.Foreach9(crIdx, [&](Monster* m)->bool {
			// (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
			auto d = m->pos - pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (rr > dd) {
				r = m;
				return true;
			}
			return false;
		});
		return r;
	}

	inline static Monster* FindNearest(SpaceGridC<Monster, XY>& container, XY const& pos, float maxDistance) {
		auto crIdx = container.PosToCrIdx(pos);					// calc grid col row index

		float minVxxyy = maxDistance * maxDistance;
		Monster* o{};
		XY ov;

		auto& lens = gLooper.sgrdd.lens;
		auto& idxs = gLooper.sgrdd.idxs;
		for (int i = 1; i < lens.len; i++) {
			if (lens[i].radius > maxDistance) break;			// limit search range

			auto offsets = &idxs[lens[i - 1].count];
			auto size = lens[i].count - lens[i - 1].count;
			container.ForeachCells(crIdx, offsets, size, [&](Monster* m)->bool {
				auto v = m->pos - pos;
				if (auto xxyy = v.x * v.x + v.y * v.y; xxyy < minVxxyy) {
					minVxxyy = xxyy;
					o = m;
					ov = v;
				}
				return false;
			});

			if (o) return o;									// found. stop ring diffuse step
		}
		return nullptr;
	}

	template<typename F>
	inline static void ForeachByRange(SpaceGridC<Monster, XY>& container, XY const& pos, float maxDistance, F&& func) {
		auto crIdx = container.PosToCrIdx(pos);					// calc grid col row index

		float rr = maxDistance * maxDistance;

		auto& lens = gLooper.sgrdd.lens;
		auto& idxs = gLooper.sgrdd.idxs;
		for (int i = 1; i < lens.len; i++) {
			if (lens[i].radius > maxDistance) break;			// limit search range

			auto offsets = &idxs[lens[i - 1].count];
			auto size = lens[i].count - lens[i - 1].count;
			container.ForeachCells(crIdx, offsets, size, [&](Monster* m)->bool {
				auto v = m->pos - pos;
				if (v.x * v.x + v.y * v.y < rr) {
					func(m);
				}
				return false;
			});
		}
	}
};

struct Monster_Dragon_BabyWhite : Monster {
	constexpr static char const* cResPrefix{ "dragon_babywhite_" };
	constexpr static XY cAnchor{ 0.5, 0.3 };
	constexpr static float cRadius{ 5 };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };
	constexpr static float cSpeed{ 30.f / gDesign.fps };

	void Init(int hp_, XY const& pos_);
	xx::Task<> MainLogic_();
};
