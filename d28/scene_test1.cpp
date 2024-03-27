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

		em.Init(&gLooper.frameNumber, &camera, &gLooper.rnd, gRes.td_effect_1);

		tasks.Add([this]()->xx::Task<> {

			while (true) {
				XY pos{};
				pos.x = gLooper.rnd.Next<float>(-gLooper.windowSize_2.x + 100, gLooper.windowSize_2.x - 100);
				pos.y = gLooper.rnd.Next<float>(-gLooper.windowSize_2.y + 100, gLooper.windowSize_2.y - 100);
				float radius = gLooper.rnd.Next<float>(16, 32);
				//int32_t count = gLooper.rnd.Next<int32_t>(64, 512);
				int32_t count = 1024;
				em.Add(pos, radius, count, 2.f, gLooper.rnd.Next<bool>());

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
}
