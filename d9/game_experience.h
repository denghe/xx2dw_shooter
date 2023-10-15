#pragma once
#include "game_drawable_sprite.h"
#include "game_hero.h"

struct Experience : Sprite, Removeable<Experience>, SpaceGridCItem<Experience, XY> {
	constexpr static char const* cResPrefix{ "icon_gem_" };
	constexpr static float cFrameInc{ 30.f / gDesign.fps };
	constexpr static float cScale{ 0.5f };
	constexpr static float cRadius{ 6.f * cScale };
	constexpr static float cSpeed{ 100.f / gDesign.fps };

	float speed{ cSpeed };
	int point{};
	xx::Weak<Hero> hero;

	void Init(XY const& pos_, int iconIndex, int point_) {
		pos = pos_;
		point = point_;
		scale = { cScale,cScale };
		radius = cRadius;
		frames = &gLooper.frames_icon_gem;
		frameIndex = (float)iconIndex;
		SGCAdd(gLooper.experiencesGrid, pos);
	}

	void FlyTo(Hero* hero_) {
		mainLogic = MainLogic_();
		hero = xx::WeakFromThis(hero_);
		SGCRemove();
		gLooper.flyingExperiences.Emplace(xx::SharedFromThis(this));
		RemoveFromOwner();
	}

	xx::Task<> MainLogic_() {
		while (hero) {
			auto d = hero->pos - pos;
			auto dd = d.x * d.x + d.y * d.y;
			if (dd > speed * speed) {
				pos += d / std::sqrt(dd) * speed;
			} else {
				// todo: hero.eat ( this )
				co_return;
			}
			co_yield 0;
		}

		auto p = xx::SharedFromThis(this);
		SGCAdd(gLooper.experiencesGrid, pos);
		container = &gLooper.experiences;
		container->Emplace(p);
		indexAndVersionByContainer = container->Tail();
	}

};
