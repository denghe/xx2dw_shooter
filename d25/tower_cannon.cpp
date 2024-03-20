#include "pch.h"
#include "scene_play.h"

namespace Tower {

	void Cannon::Init(int32_t colIdx, int32_t rowIdx) {
		//draw = [](void* self) { ((Arrow*)self)->Draw(); };
		// todo
	}

	bool Cannon::Update() { return false; }

	void Cannon::Draw() {
		auto& camera = gScenePlay->camera;
		{
			auto& q = Quad::DrawOnce(gRes.td_tower_base);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = XY::Make(camera.scale);
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
		{
			auto& q = Quad::DrawOnce(gRes.td_icon_cannon1);
			q.pos = camera.ToGLPos(pos);
			q.anchor = cAnchor;
			q.scale = XY::Make(camera.scale);
			q.radians = 0;
			q.colorplus = 1;
			q.color = RGBA8_White;
		}
	}

}
