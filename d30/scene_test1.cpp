#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"
#include "xx_blocklink.h"

namespace Test1 {
	void Scene::Init() {
		gScene = this;

		rootNode.Emplace()->Init();
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
			gLooper.DelaySwitchTo<SceneMainMenu>();
			}, 3);

		// init camera
		camera.SetMaxFrameSize(gCfg.unitSizef);
		camera.SetOriginal(gCfg.mapSize_2);
		camera.SetScale(1.f);

		// init grids & data
		grids.InitAll(gCfg.numGridRows, gCfg.numGridCols, gCfg.unitSizei);

		grids.Get<Hero>().EmplaceInit();
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
		grids.Foreach([&]<typename T>(SGS::SG<T>& grid) {
			grid.Foreach([](auto& o)->FR {
				return o.Update() ? FR::RemoveAndContinue : FR::Continue;
			});
		});
	}

	void Scene::Draw() {
		camera.Calc();

		grids.Foreach([&]<typename T>(SGS::SG<T>&grid) {
			grid.Foreach([&](auto& o) {
				if (camera.InArea(o.pos)) {
					bases.Emplace(&o);
				}
			});
		});
		bases.StdSort([](Base* a, Base* b) { return a->pos.y < b->pos.y; });
		for (auto& o : bases) {
			o->Draw();
		}
		bases.Clear();

		gLooper.DrawNode(rootNode);
	}




	// todo: more args
	XX_FORCE_INLINE QuadInstanceData& Base::BaseDraw() {
		auto& camera = gScene->camera;
		auto& q = Quad::DrawOnce(gRes.egg_blue);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };			// todo set by res
		q.scale = camera.scale * radius * gCfg._1_unitSizef;
		q.radians = 0;
		q.colorplus = 1;
		q.color = { 255, 255, 255, 255 };
		return q;
	}



	void Hero::Init() {
		pos = gCfg.mapSize_2;
		radius = gCfg.unitSizef;
	}

	int Hero::Update() {
		return 0;
	}

	void Hero::Draw() {
		BaseDraw();
	}

	int Pet::Update() {
		return 0;
	}

	void Pet::Draw() {
		BaseDraw();
	}

	int Monster::Update() {
		return 0;
	}

	void Monster::Draw() {
		BaseDraw();
	}

}
