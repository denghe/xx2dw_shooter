#include "pch.h"
#include "scene_play.h"

namespace Enemy {
	void Monster2::Init(double hp_, int32_t mapPathIndex_) {
		//draw = [](void* self) { ((Monster2*)self)->Draw(); };
		hpBak = hp = hp_;
		//hp *= (double)gScenePlay->rnd.Next<float>(0.01f, 0.99f);
		radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
		mapPathIndex = mapPathIndex_;
		auto& tm = gScenePlay->tms[mapPathIndex_];
		assert(radius <= tm.totalWidth);
		assert(radius >= tm.trackMargin);
		//auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
		//auto range = (tm.trackCount - numTrackCovered);
		//trackIndex = numTrackCovered / 2 + gScenePlay->rnd.Next<int32_t>(range);
		trackIndex = gScenePlay->rnd.Next<int32_t>(0, tm.trackCount - 1);
		pointIndex = {};
		speed = cSpeed * cRadius / radius;
		radians = tm.GetRadians((int)pointIndex);
		pos = tm.GetPoint(trackIndex, (int)pointIndex);
	}

	bool Monster2::Update() {
		// todo: hit tower logic?
		auto& tm = gScenePlay->tms[mapPathIndex];
		pointIndex += speed;
		if (auto c = tm.GetPointCount(); pointIndex >= c) {
			// todo: damage player? switch to another state? change to another monster?
			return true;
		}
		pos = tm.GetPoint(trackIndex, (int)pointIndex);
		gScenePlay->grids.Update(*this);
		radians = tm.GetRadians((int)pointIndex);
		return false;
	}

	void Monster2::Draw() {
		auto& camera = gScenePlay->camera;
		auto& q = Quad::DrawOnce(gRes.td_shape_circle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = camera.scale * (radius / cRadius);
		q.radians = radians;
		q.colorplus = 1;
		q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
	}

	void Monster2::PlayDeathEffect() {
		// play effect
		gScenePlay->em.Add(pos, cRadius, 256, 1.5f);
	}
}
