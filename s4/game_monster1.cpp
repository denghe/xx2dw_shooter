#include "pch.h"

void Monster1::Draw() {
	body.SetFrame(gLooper.frames_monster_1[(int32_t)frameIndex])
		.SetScale(scale * gLooper.camera.scale)
		.SetPosition(gLooper.camera.ToGLPos(pos))
		.Draw();
}

void Monster1::Init(XY const& bornPos) {
	mainLogic = MainLogic();
	radius = cRadius;
	pos = bornPos;
	GridInit();
}

void Monster1::Hit(int damage) {
	assert(damage > 0);
	if (damage >= hp) {
		// play effects   
		gLooper.effects_damageText.Emplace().Emplace()->Init(pos, hp, { 255,0,0,255 });
		gLooper.effects_explosion.Emplace().Emplace()->Init(pos);
		RemoveFromOwner();		// kill self
	} else {
		hp -= damage;
		float dispersedVal = 20 / gScale * scale;
		gLooper.effects_damageText.Emplace().Emplace()->Init({ gLooper.rnd.Next(pos.x - dispersedVal, pos.x + dispersedVal), gLooper.rnd.Next(pos.y - dispersedVal, pos.y + dispersedVal) }, damage, {255,255,0,255});
		if (hertLife <= 0) {
			hitLogic(gLooper.tasks, [this]()->xx::Task<> {
				while (hertLife) {
					--hertLife;
					body.SetColormulti(255);
					co_yield 0;
				}
				body.SetColormulti(1);
			});
		}
		hertLife = cHertLife;
	}
}

xx::Task<> Monster1::MainLogic() {
	float tarScale = gLooper.rnd.Next<float>(cScale, cMaxScale);
	float scaleStep = tarScale / cScaleSteps;
	// scale in
	body.SetScale(0);
	while (scale < tarScale) {
		scale += scaleStep;
		radius = scale / cScale * cRadius;
		co_yield 0;
	}
	scale = tarScale;
	radius = scale / cScale * cRadius;

	// follow shooter
	while (--life > 0) {
		if (hertLife > 0) co_yield 0;							// when hert, can't move

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
	while (scale > scaleStep) {
		scale -= scaleStep;
		radius = scale / cScale * cRadius;
		co_yield 0;
	}
}
