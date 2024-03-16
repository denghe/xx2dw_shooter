#include "pch.h"
#include "scene_play.h"

namespace Enemy {
	void Monster2::Init(double hp_, int32_t mapPathIndex_) {
		//draw = [](void* self) { ((Monster2*)self)->Draw(); };
		hpBak = hp = hp_;
		//hp *= (double)gSceneTest2->rnd.Next<float>(0.01f, 0.99f);
		radius = (float)std::sqrt(cRadius * cRadius / cHP * hp_);
		mapPathIndex = mapPathIndex_;
		auto& tm = gSceneTest2->tms[mapPathIndex_];
		assert(radius <= tm.totalWidth);
		assert(radius >= tm.trackMargin);
		//auto numTrackCovered = int32_t(radius * 2 / tm.trackMargin);
		//auto range = (tm.trackCount - numTrackCovered);
		//trackIndex = numTrackCovered / 2 + gSceneTest2->rnd.Next<int32_t>(range);
		trackIndex = gSceneTest2->rnd.Next<int32_t>(0, tm.trackCount - 1);
		pointIndex = {};
		speed = cSpeed * cRadius / radius;
		radians = tm.GetRadians((int)pointIndex);
		pos = tm.GetPoint(trackIndex, (int)pointIndex);
	}

	bool Monster2::Update() {
		// todo: hit tower logic?
		auto& tm = gSceneTest2->tms[mapPathIndex];
		pointIndex += speed;
		if (auto c = tm.GetPointCount(); pointIndex >= c) {
			// todo: damage player? switch to another state? change to another monster?
			return true;
		}
		gSceneTest2->grids.Get<::Enemy::Monster2>().Update(*this, tm.GetPoint(trackIndex, (int)pointIndex));
		radians = tm.GetRadians((int)pointIndex);
		return false;
	}

	void Monster2::Draw() {
		auto& camera = gSceneTest2->camera;
		auto& q = Quad::DrawOnce(gLooper.frame_td_shape_circle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale) * (radius / cRadius);
		q.radians = radians;
		q.colorplus = 1;
		q.color = { cColor.r, cColor.g, cColor.b, uint8_t(40 + 160 * (hp / hpBak)) };
	}
}
