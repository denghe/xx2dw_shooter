#include "pch.h"
#include "scene_play.h"

namespace Bullet::Tower {

	void Arrow::Init(::Tower::Arrow& owner, MonsterBase& tar) {
		//draw = [](void* self) { ((Arrow*)self)->Draw(); };
		damage = owner.damage;
		deathFrameNumber = gLooper.frameNumber + (int32_t)(cTimeSpan / gDesign.frameDelay);
		radius = cRadius * cScale;
		speed = cSpeed;
		pos = owner.pos;

		// calc target's pos
		auto dist = Calc::Distance(owner.pos, tar.pos);
		auto pointIndex = int(tar.pointIndex + tar.speed * (dist / cSpeed));
		auto& tm = gScenePlay->tms[tar.mapPathIndex];
		if (auto c = tm.GetPointCount(); pointIndex >= c) {
			pointIndex = c - 1;
		}
		auto tarPos = tm.GetPoint(tar.trackIndex, pointIndex);
		auto d = tarPos - owner.pos;
		radians = std::atan2(d.y, d.x);
		inc.x = std::cos(radians) * speed;
		inc.y = std::sin(radians) * speed;
	}

	bool Arrow::Update() {
		// life span check
		if (deathFrameNumber <= gLooper.frameNumber) return true;

		// hit check
		bool death = false;
		gScenePlay->grids.Get<::Enemy::Monster2>().Foreach9(pos, [&](::Enemy::Monster2& o)->GridForeachResult {
			// intersects ?
			if (Calc::Intersects::CircleCircle<float>(
				pos.x, pos.y, radius, o.pos.x, o.pos.y, o.radius)) {
				death = true;
				gScenePlay->enm.Add(pos, pos - o.pos, { 255,0,0,255 }, (int32_t)damage);
				o.hp -= damage;
				if (o.hp <= 0) {
					// todo: play monster death effect ?
					return GridForeachResult::RemoveAndBreak;
				}
				return GridForeachResult::Break;
			}
			return GridForeachResult::Continue;
			});
		if (death) {
			// todo: play effect ?
			return true;
		}

		// calc move pos
		auto newPos = pos + inc;

		// edge check
		if (newPos.x < 0 || newPos.x >= gScenePlay->mapMaxX ||
			newPos.y < 0 || newPos.y >= gScenePlay->mapMaxY) return true;

		// move
		gScenePlay->grids.Update(*this, newPos);

		return false;
	}

	void Arrow::Draw() {
		auto& camera = gScenePlay->camera;
		{
			// tail
			auto& q = Quad::DrawOnce(gLooper.frame_td_shape_trangle);
			q.pos = camera.ToGLPos(pos);
			q.anchor = { 0.5f, 1.f };
			q.scale = XY::Make(camera.scale) * cScale * XY { 1.f, cTailRatio };
			q.radians = radians + gPI / 2;
			q.colorplus = 1;
			q.color = RGBA8_White;
			q.color.a = 127;
		}
		{
			// body
			auto& q = Quad::DrawOnce(gLooper.frame_td_shape_circle);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = XY::Make(camera.scale) * cScale;
			q.radians = radians;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
	}

}
