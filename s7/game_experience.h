#pragma once
#include "game_drawable_sprite.h"
#include "game_hero.h"

struct Experience : Sprite {
	constexpr static char const* cResPrefix{ "icon_gem_" };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cScale{ 0.5f };
	constexpr static float cRadius{ 6.f * cScale };
	constexpr static float cSpeed{ 100.f / gDesign.fps };

	float speed{ cSpeed };

	void Init(XY const& pos_, int iconIndex, int point) {
		mainLogic = MainLogic_();
		pos = pos_;
		scale = { cScale,cScale };
		radius = cRadius;
		frames = &gLooper.frames_icon_gem;
		frameIndex = iconIndex;
	}

	// todo: wait for hero find & eat. grid index

	xx::Task<> MainLogic_() {
		auto& heros = gLooper.heros;
		do {
			co_yield 0;
			if (heros.Empty()) continue;

			auto hero = heros[heros.head].ToWeak();			// todo: find nearest hero?
			auto rr = (hero->eatExperienceDistance + radius) * (hero->eatExperienceDistance + radius);
			auto d = hero->pos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (dd < rr) {
				while(true) {								// fly to hero
					if (dd > speed * speed) {
						pos += d / std::sqrt(dd) * speed;
					} else {
						co_return;	// todo: eat
					}
					co_yield 0;
					if (!hero) break;
					d = hero->pos - pos;
					dd = d.x * d.x + d.y + d.y;
				}
			}
		} while (true);
	}

};
