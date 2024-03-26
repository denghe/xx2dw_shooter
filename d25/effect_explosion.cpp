#include "pch.h"
#include "scene_play.h"

namespace Effect {

	void ExplosionManager::Init(int32_t cap) {
		items.Reserve(cap);
	}

	void ExplosionManager::Add(XY const& pos_, float radius, int32_t count, float timeScale) {
		auto& rnd = gLooper.rnd;
		items.Reserve(items.Count() + 16);

		for (int32_t i = 0; i < count; i++) {
			auto& o = items.Emplace();
			auto r = std::sqrt(rnd.Next<float>()) * radius;
			auto a = rnd.Next<float>(gNPI, gPI);
			auto c = std::cos(a);
			auto s = std::sin(a);
			o.radians = a;
			o.pos.x = pos_.x + c * r;
			o.pos.y = pos_.y + s * r;
			auto speed = rnd.Next<float>(50, 200) / gDesign.fps;
			o.inc.x = c * speed;
			o.inc.y = s * speed;
			o.beginFrameNumber = gScenePlay->frameNumber;
			o.endFrameNumber = gScenePlay->frameNumber + int32_t(rnd.Next<float>(0.3f, 0.6f) * timeScale / gDesign.frameDelay);
			o.color = { 255,255,255,255 };	// todo: rnd ?
		}
	}

	void ExplosionManager::Update() {
		auto& camera = gScenePlay->camera;
		auto count = items.Count();
		int32_t i = 0;
		auto fn = gScenePlay->frameNumber;
		for (auto i = items.len - 1; i >= 0; --i) {
			auto& o = items[i];
			if (o.endFrameNumber <= fn) {
				items.SwapRemoveAt(i);
			} else {
				o.pos += o.inc;
				o.color.a = (uint8_t)(255 * (1.f - float(fn - o.beginFrameNumber) / (o.endFrameNumber - o.beginFrameNumber)));
			}
		}
	}

	void ExplosionManager::Draw() {
		auto& camera = gScenePlay->camera;
		auto count = items.len;
		auto& f = *gRes.td_effect_1;
		auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), count);
		for (auto i = 0; i < count; ++i) {
			auto& o = items[i];
			auto& q = qs[i];
			q.pos = camera.ToGLPos(o.pos);
			q.anchor = { 0.5f, 0.5f };
			q.scale = camera.scale/* * scale*/; // rnd?
			q.radians = o.radians;
			q.colorplus = 1;
			q.color = o.color;
			q.texRect = f.textureRect;
		}
	}

}
