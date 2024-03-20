#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"

namespace Test1 {

	void Scene::Init() {
		gScene = this;

		rootNode.Emplace()->Init();
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
			gLooper.DelaySwitchTo<SceneMainMenu>();
		});

		camera.SetScale(1.f);

		em.Init();

		tasks.Add([this]()->xx::Task<> {

			while (true) {
				XY pos{};
				pos.x = gLooper.rnd.Next<float>(-gLooper.windowSize_2.x + 100, gLooper.windowSize_2.x - 100);
				pos.y = gLooper.rnd.Next<float>(-gLooper.windowSize_2.y + 100, gLooper.windowSize_2.y - 100);
				float radius = gLooper.rnd.Next<float>(16, 32);
				//int32_t count = gLooper.rnd.Next<int32_t>(64, 512);
				int32_t count = 1024;
				em.Add(pos, radius, count);

				//co_await gLooper.AsyncSleep(0.5f);
				co_yield 0;
			}

		});
	}

	void Scene::Update() {
		// scale control
		if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
			camera.IncreaseScale(0.1f, 5);
		} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
			camera.DecreaseScale(0.1f, 0.1f);
		}
		camera.Calc();

		em.Update();
	}

	void Scene::Draw() {
		camera.Calc();

		em.Draw();

		auto str = xx::ToString("total partical count = ", em.items.len);
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

		gLooper.DrawNode(rootNode);
	}


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
			o.beginFrameNumber = gLooper.frameNumber;
			o.endFrameNumber = gLooper.frameNumber + int32_t(rnd.Next<float>(0.3f, 0.6f) * timeScale / gDesign.frameDelay);
			o.color = { 255,255,255,255 };	// todo: rnd ?
		}
	}

	void ExplosionManager::Update() {
		auto& camera = gScene->camera;
		auto count = items.Count();
		int32_t i = 0;
		auto fn = gLooper.frameNumber;
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
		auto& camera = gScene->camera;
		auto count = items.len;
		auto& f = *gLooper.frame_td_effect_1;
		auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), count);
		for (auto i = 0; i < count; ++i) {
			auto& o = items[i];
			auto& q = qs[i];
			q.pos = camera.ToGLPos(o.pos);
			q.anchor = { 0.5f, 0.5f };
			q.scale = XY::Make(camera.scale)/* * scale*/; // rnd?
			q.radians = o.radians;
			q.colorplus = 1;
			q.color = o.color;
			q.texRect = f.textureRect;
		}
	}
}
