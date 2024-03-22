#include "pch.h"
#include "effect_explosion.h"

namespace Effect {

	void ExplosionManager::Init(int32_t* frameNumber_, Camera* camera_, Rnd* rnd_, xx::Ref<Frame> frame_, RGBA8 color_, int32_t cap) {
		frameNumber = frameNumber_;
		camera = camera_;
		color = color_;
		rnd = rnd_;
		frame = frame_;
		items.Reserve(cap);
	}

	void ExplosionManager::Add(XY const& pos_, float radius, int32_t count, float timeScale, bool rndRadians) {
		items.Reserve(items.Count() + 16);

		for (int32_t i = 0; i < count; i++) {
			auto& o = items.Emplace();
			auto r = std::sqrt(rnd->Next<float>()) * radius;
			auto a = rnd->Next<float>(gNPI, gPI);
			auto c = std::cos(a);
			auto s = std::sin(a);
			o.pos.x = pos_.x + c * r;
			o.pos.y = pos_.y + s * r;
			if (rndRadians) {
				a = rnd->Next<float>(gNPI, gPI);
				c = std::cos(a);
				s = std::sin(a);
			}
			o.radians = a;
			auto speed = rnd->Next<float>(50, 200) / gDesign.fps;
			o.inc.x = c * speed;
			o.inc.y = s * speed;
			o.beginFrameNumber = *frameNumber;
			o.endFrameNumber = *frameNumber + int32_t(rnd->Next<float>(0.3f, 0.6f) * timeScale / gDesign.frameDelay);
			o.color = color;
		}
	}

	void ExplosionManager::Update() {
		auto count = items.Count();
		int32_t i = 0;
		for (auto i = items.len - 1; i >= 0; --i) {
			auto& o = items[i];
			if (o.endFrameNumber <= *frameNumber) {
				items.SwapRemoveAt(i);
			} else {
				o.pos += o.inc;
				o.color.a = (uint8_t)(255 * (1.f - float(*frameNumber - o.beginFrameNumber) / (o.endFrameNumber - o.beginFrameNumber)));
			}
		}
	}

	void ExplosionManager::Draw() {
		auto& e = EngineBase3::Instance();
		auto& s = e.shaderQuadInstance;
		auto count = items.len;
		int32_t j{};
	LabLoop:
		auto left = s.maxQuadNums - s.quadCount;
		auto siz = std::min( left ? left : s.maxQuadNums, count);
		auto qs = e.ShaderBegin(s).Draw(frame->tex->GetValue(), siz);
		for (int32_t i = 0; i < siz; ++i) {
			auto& o = items[j + i];
			auto& q = qs[i];
			q.pos = camera->ToGLPos(o.pos);
			q.anchor = { 0.5f, 0.5f };
			q.scale = XY::Make(camera->scale);
			q.radians = o.radians;
			q.colorplus = 1;
			q.color = o.color;
			q.texRect = frame->textureRect;
		}
		j += siz;
		count -= siz;
		if (count > 0) goto LabLoop;
	}

}
