#include "pch.h"
#include "scene_play.h"

namespace Tower {

	void Arrow::Init(int32_t colIdx, int32_t rowIdx) {
		pos.x = colIdx * gCfg.unitSize + gCfg.unitSize / 2;
		pos.y = rowIdx * gCfg.unitSize + gCfg.unitSize / 2;
		damage = cDamage;
	}

	bool Arrow::Update() {
		// check cast delay
		if (nextFireFrame < gLooper.frameNumber) {
			nextFireFrame = gLooper.frameNumber + cFireAfterDelayFrame;

			// find most dangerous enemy in attack area
			::Enemy::Monster2* tar{};
			gSceneTest2->grids.Get<::Enemy::Monster2>().ForeachByRange(gLooper.sgrdd, pos
				, cAttackRange + gCfg.unitSize / 2, [&](::Enemy::Monster2& o) {
					if (!tar) {
						tar = &o;
					} else {
						if (o.pointIndex > tar->pointIndex) {
							tar = &o;
						}
					}
				});

			// fire
			if (tar) {
				gSceneTest2->grids.MakeInit<::Bullet::Tower::Arrow>(*this, *tar);
			}
		}
		return false;
	}

	void Arrow::Draw() {
		auto& camera = gSceneTest2->camera;
		{
			auto& q = Quad::DrawOnce(gLooper.frame_td_tower_base);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = XY::Make(camera.scale);
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
		{
			auto& q = Quad::DrawOnce(gLooper.frame_td_icon_arrow1);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = XY::Make(camera.scale);
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
	}

	void Arrow::Focus() {
		auto& camera = gSceneTest2->camera;

		auto& q = Quad::DrawOnce(gLooper.frame_td_cell_mouse_focus);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale);
		q.radians = 0;
		q.colorplus = 1;
		q.color = { 255,0,0,255 };
		gLooper.ShaderEnd();

		auto bak = gLooper.blend;
		gLooper.GLBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_COLOR, GL_FUNC_ADD });

		RingInstanceData rid;
		rid.pos = camera.ToGLPos(pos);
		rid.color = { 255,255,255,255 };
		rid.radius = camera.scale * cAttackRange;
		gSceneTest2->ringShader.DrawOne(rid);

		gLooper.GLBlendFunc(bak);
	}

}
