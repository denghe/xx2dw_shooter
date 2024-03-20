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
		camera.SetMaxFrameSize({ gCfg.unitSize, gCfg.unitSize });
		camera.SetOriginal(gCfg.mapCenterPos);

		em.Init(&frameNumber, &camera, &rnd, gRes.td_effect_1);

		mainTask = MainTask();

		gameSpeedRate = 1;

		walls.Emplace().Init(gCfg.mapCenterPos
			+ XY{ -gCfg.mapSize_2.x - gCfg.unitSize_2, 0 }, { gCfg.unitSize, gCfg.mapSize.y });

		walls.Emplace().Init(gCfg.mapCenterPos
			+ XY{ gCfg.mapSize_2.x + gCfg.unitSize_2, 0 }, { gCfg.unitSize, gCfg.mapSize.y });

		walls.Emplace().Init(gCfg.mapCenterPos
			+ XY{ 0, -gCfg.mapSize_2.y - gCfg.unitSize_2 }, { gCfg.mapSize.x + gCfg.unitSize * 2, gCfg.unitSize });
	}

	xx::Task<> Scene::MainTask() {
		while (true) {

			//XY pos{};
			//pos.x = gLooper.rnd.Next<float>(-gLooper.windowSize_2.x + 100, gLooper.windowSize_2.x - 100);
			//pos.y = gLooper.rnd.Next<float>(-gLooper.windowSize_2.y + 100, gLooper.windowSize_2.y - 100);
			//float radius = gLooper.rnd.Next<float>(16, 32);
			//int32_t count = gLooper.rnd.Next<int32_t>(64, 512);
			//em.Add(pos, radius, count);

			//co_await gLooper.AsyncSleep(0.5f);
			co_yield 0;
		}
	}

	void Scene::BeforeUpdate() {
		// scale control
		if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
			camera.IncreaseScale(0.1f, 5);
		} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
			camera.DecreaseScale(0.1f, 0.1f);
		}
		camera.Calc();
	}

	void Scene::Update() {
		for (int32_t i = 0; i < gameSpeedRate; ++i) {
			++frameNumber;
			mainTask();
			em.Update();
		}
	}

	void Scene::Draw() {

		for (auto& wall : walls) {
			wall.Draw();
		}

		// ...

		em.Draw();

		//auto str = xx::ToString("total partical count = ", em.items.len);
		//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

		gLooper.DrawNode(rootNode);
	}

	/******************************************************************************************************/
	/******************************************************************************************************/


	void Wall::Init(XY const& pos_, XY const& size_) {
		pos = pos_;
		size = size_;
		auto tmp = size / 2;
		xy.from = pos - tmp;
		xy.to = pos + tmp;
	}

	void Wall::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_block;
		auto& q = Quad::DrawOnce(frame);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { size.x / frame->textureRect.w * camera.scale, size.y / frame->textureRect.h * camera.scale };
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}

}
