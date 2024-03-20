#include "pch.h"
#include "scene_play.h"

namespace Tower {

	void Arrow::Init(int32_t colIdx, int32_t rowIdx) {
		pos.x = colIdx * gCfg.unitSize + gCfg.unitSize / 2;
		pos.y = rowIdx * gCfg.unitSize + gCfg.unitSize / 2;
		damage = cDamage;

		// init anims
		InitShootAnim();
		InitFocusAnim();
	}

	bool Arrow::Update() {
		// check cast delay
		if (nextFireFrame < gScenePlay->frameNumber) {
			nextFireFrame = gScenePlay->frameNumber + cFireAfterDelayFrame;

			// find most dangerous enemy in attack area
			::Enemy::Monster2* tar{};
			gScenePlay->grids.Get<::Enemy::Monster2>().ForeachByRange(gLooper.sgrdd, pos
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
				gScenePlay->grids.MakeInit<::Bullet::Tower::Arrow>(*this, *tar);
				ResetShootAnim();	// reset shoot anim
			}
		}
		
		if (case_shootScale >= 0) {
			StepShootAnim();		// play shoot anim
		}
		return false;
	}

	void Arrow::Draw() {
		auto& camera = gScenePlay->camera;
		auto s = XY::Make(camera.scale) * (2.f - focusScale) * shootScale;
		{
			auto& q = Quad::DrawOnce(gRes.td_tower_base);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = s;
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
		{
			auto& q = Quad::DrawOnce(gRes.td_icon_arrow1);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = s;
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
	}

	void Arrow::DrawFocus() {
		auto& camera = gScenePlay->camera;

		auto& q = Quad::DrawOnce(gRes.td_cell_mouse_focus);
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
		rid.radius = camera.scale * cAttackRange * focusScale;
		gScenePlay->ringShader.DrawOne(rid);

		gLooper.GLBlendFunc(bak);
	}

}
