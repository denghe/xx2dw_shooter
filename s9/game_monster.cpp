#include "pch.h"

void Monster_Dragon_BabyWhite::Init(int hp_, XY const& pos_) {
	mainLogic = MainLogic_();
	idle = Idle_Rotate();
	hp = hp_;
	pos = pos_;
	scale = { cScale, 0 };
	speed = cSpeed;
	radius = cRadius;
	SGCAdd(gLooper.monstersGrid, pos);
	frames = &gLooper.frames_dragon_babywhite;
	body.SetAnchor(cAnchor);
}

xx::Task<> Monster_Dragon_BabyWhite::MainLogic_() {
	float frameMaxIndex = frames->size();

	// scale in
	auto scaleStep = cScale / gDesign.fps;
	while (scale.y < cScale) {
		scale.y += scaleStep;
		// todo: set visible 
		co_yield 0;
	}
	scale.y = cScale;

	xx::Weak<Hero> tarHero;
	while (true) {
		if (auto h = gLooper.GetNearestHero(pos)) {
			tarHero = xx::WeakFromThis(h);
			break;
		}
		idle();
		co_yield 0;
	}

	while (tarHero) {
		if (damageEffectLeftDuration > 0) co_yield 0;			// when hert, can't move

		// physics simulate
		// calc neighbor cross force
		XY combineForce{};										// for move vector
		int numCross{}, limit = 8;
		auto crIdx = _sgc->PosToCrIdx(pos);						// calc grid col row index
		_sgc->Foreach9(crIdx, [&](Monster* m) {
			if (m == this) return false;						// skip self
			auto d = pos - m->pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (rr > dd) {										// cross?
				++numCross;
				if (dd) {
					combineForce += d / std::sqrt(dd);			// normalize
				}
			}
			return --limit < 0;									// number limit
		});

		// calc follow force
		// todo: find hero follow ? or find nearest ?
		auto d = tarHero->pos - pos;
		auto dd = d.x * d.x + d.y * d.y;

		// calc new pos
		XY newPos = pos;
		if (numCross) {											// cross?
			if (dd) {
				combineForce += d / std::sqrt(dd) / 100;		// weak force assign for ship follow
			}
			if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
				auto r = gEngine->rnd.Next<float>(M_PI * 2);
				newPos += XY{ std::cos(r), std::sin(r) } *cSpeed * 3;
			} else {
				newPos += (combineForce.MakeNormalize() - XY{0.32, 0})* cSpeed;	// 0.32 for fix inhomogeneous
			}
		} else {
			if (dd > cSpeed * cSpeed) {							// follow shooter directly
				newPos += d / std::sqrt(dd) * cSpeed;			// normalize
			} else {
				newPos = tarHero->pos;
			}
		}

		//// get box & fix newPos
		//auto& sg = gLooper.sgabTrees;
		//auto minXY = (pos - cRadius).As<int32_t>(), maxXY = (pos + cRadius).As<int32_t>();
		//if (minXY.x < 0) minXY.x = 0;
		//if (minXY.y < 0) minXY.y = 0;
		//if (maxXY.x >= sg.maxX) maxXY.x = sg.maxX - 1;
		//if (maxXY.y >= sg.maxY) maxXY.y = sg.maxY - 1;
		//if (minXY.x < maxXY.x && minXY.y < maxXY.y) {
		//	sg.ForeachAABB(minXY, maxXY);
		//	auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
		//	for (auto& b : sg.results) {
		//		TranslateControl::MoveCircleIfIntersectsBox<float>(b->_sgabPos.x, b->_sgabPos.y, Tree::cBoxWidth / 2, Tree::cBoxHeight / 2, newPos.x, newPos.y, radius);
		//	}
		//}

		// upgrade space grid
		if (newPos != pos) {
			pos = newPos;
			SGCUpdate(pos);
			FrameControl::Forward(frameIndex, cFrameInc * (speed / cSpeed), frameMaxIndex);
		} else {
			idle();
		}

		co_yield 0;
	}

	// scale out
	while (scale.y > 0.f) {
		scale.y -= scaleStep;
		// todo: set visible 
		co_yield 0;
	}
	scale.y = 0;
}
