#include "pch.h"

void Monster1::Init(XY const& bornPos) {
	radius = cRadius;
	Add(MainLogic());
	pos = bornPos;
	GridInit();
	body.SetColormulti(255);
}

void Monster1::Draw() {
	body.TrySetFrame(gLooper.frames_monster_1[(int32_t)frameIndex])
		.SetScale(scale * gLooper.camera.scale)
		.SetPosition(gLooper.camera.ToGLPos(pos))
		.Draw();
}

xx::Task<> Monster1::MainLogic() {
	// scale in
	body.SetScale(0);
	while (scale < cScale) {
		scale += cScaleStep;
		radius = scale / cScale * cRadius;
		co_yield 0;
	}
	scale = cScale;
	radius = cRadius;

	// follow shooter
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

		// calc ship follow force
		auto d = gLooper.shooter->pos - pos;
		auto dd = d.x * d.x + d.y * d.y;

		// calc new pos
		XY newPos = pos;
		if (numCross) {											// cross?
			if (dd) {
				combineForce += d / std::sqrt(dd) / 100;		// weak force assign for ship follow
			}
			if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
				auto r = gLooper.rnd.Next<float>(M_PI * 2);
				newPos += XY{ std::cos(r), std::sin(r) } * cSpeed * 3;
			} else {
				newPos += combineForce.MakeNormalize() * cSpeed;
			}
		} else {
			if (dd > cSpeed * cSpeed) {							// follow shooter directly
				newPos += d / std::sqrt(dd) * cSpeed;			// normalize
			} else {
				newPos = gLooper.shooter->pos;
			}
		}

		// get box & fix newPos
		auto& sg = gLooper.sgabTrees;
		auto minXY = (pos - cRadius).As<int32_t>(), maxXY = (pos + cRadius).As<int32_t>();
		if (minXY.x < 0) minXY.x = 0;
		if (minXY.y < 0) minXY.y = 0;
		if (maxXY.x >= sg.maxX) maxXY.x = sg.maxX - 1;
		if (maxXY.y >= sg.maxY) maxXY.y = sg.maxY - 1;
		if (minXY.x < maxXY.x && minXY.y < maxXY.y) {
			sg.ForeachAABB(minXY, maxXY);
			auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
			for (auto& b : sg.results) {
				MoveCircleIfIntersectsBox<float>(b->_sgabPos.x, b->_sgabPos.y, Tree::cBoxWidth / 2, Tree::cBoxHeight / 2, newPos.x, newPos.y, radius);
			}
		}

		// upgrade space grid
		if (newPos != pos) {
			pos = newPos;
			GridUpdate();
		}

		co_yield 0;
	}

	// scale out
	while (scale > cScaleStep) {
		scale -= cScaleStep;
		radius = scale / cScale * cRadius;
		co_yield 0;
	}
}
