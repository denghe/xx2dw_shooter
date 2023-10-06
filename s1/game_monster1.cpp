#include "pch.h"

void Monster1::Init(XY const& bornPos) {
	type = cType;
	radius = cRadius * cScale;
	Add(MainLogic());
	scale = { cScale / gSQ, cScale / gSQ };
	pos = bornPos;
	GridInit();
}
void Monster1::Draw() {
	SetFrame(gLooper.frames_monster_1[(int32_t)frameIndex]).Draw();
}
xx::Task<> Monster1::MainLogic() {
	// scale in
	while (scale.x < cScale) {
		scale.x += cScale / 10;
		scale.y += cScale / 10;
		co_yield 0;
	}
	scale = { cScale, cScale };

	while (--life > 0) {

		// step frame anim
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}

		// physics simulate
		// calc neighbor cross force
		XY combineForce{};										// for move vector
		int numCross{}, limit = 8;
		auto p = gGridBasePos.MakeAdd(pos);						// convert pos to grid coordinate
		auto crIdx = _sgc->PosToCrIdx(p);						// calc grid col row index
		_sgc->Foreach9(crIdx, [&](MonsterBase* m) {
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
		//printf("numCross = %d  combineForce %f %f\n", numCross, combineForce.x, combineForce.y);

		// calc ship follow force
		auto d = gLooper.shooter->pos - pos;
		auto dd = d.x * d.x + d.y * d.y;

		// calc new pos
		XY newPos = pos;
		if (numCross) {											// cross?
			if (dd) {
				combineForce += d / std::sqrt(dd) / 100;		// weak force assign for ship follow
				//printf("combineForce = %f %f\n", combineForce.x, combineForce.y);
			}
			//if (combineForce.IsZero()) {						// move by random angle
			if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
				auto r = gEngine->rnd.Next<float>(M_PI * 2);
				newPos += XY{ std::cos(r), std::sin(r) } * cSpeed * 3;
				//printf("r = %f   pos = %f %f   newPos = %f %f\n", r, pos.x, pos.y, newPos.x, newPos.y);
			} else {
				newPos += combineForce.MakeNormalize() * cSpeed;
				//printf("pos = %f %f   newPos = %f %f\n", pos.x, pos.y, newPos.x, newPos.y);
			}
			//newPos += XY{ -0.5f, 0.34f };	// fix group effect	  todo: change calc step to update
		} else {
			if (dd > cSpeed * cSpeed) {							// follow shooter directly
				newPos += d / std::sqrt(dd) * cSpeed;			// normalize
			} else {
				newPos = gLooper.shooter->pos;
			}
		}

		// upgrade space grid
		if (newPos != pos) {
			//printf("pos = %f %f    newPos = %f %f\n", pos.x, pos.y, newPos.x, newPos.y);
			pos = newPos;
			GridUpdate();
		}

		co_yield 0;
	}

	// scale out
	while (scale.x > cScale / 10) {
		scale.x -= cScale / 10;
		scale.y -= cScale / 10;
		co_yield 0;
	}
}
