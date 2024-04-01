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
		camera.SetScale(0.5f);

		// init grids & data
		grids.InitAll(gCfg.numGridRows, gCfg.numGridCols, gCfg.unitSizei);

		//for (int32_t i = 1; i < 10000; ++i) {
		//	petsPos.Emplace(gLooper.srdd.idxs[i]);
		//}

		{
			int32_t n = 10000;
			auto radius = 1.5f;
			while (true) {
				auto radians = std::asin(0.5f / radius) * 2;
				auto step = (int32_t)std::floor(g2PI / radians);
				auto inc = g2PI / step;
				for (int32_t i = 0; i < step; ++i) {
					auto a = inc * i;
					petsPos.Emplace(Calc::RotatePoint({ radius, 0 }, a));
					if (--n <= 0) goto LabEnd;
				}
				radius += 1.f;
			}
		LabEnd:;
		}


		auto& hero = grids.Get<Hero>().EmplaceInit(gCfg.mapSize_2);
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
		grids.Foreach([&]<typename T>(SGS::SG<T>&grid) {
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
	XX_FORCE_INLINE QuadInstanceData& Base::BaseDraw(xx::Ref<Frame> const& f) {
		auto& camera = gScene->camera;
		auto& q = Quad::DrawOnce(f);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.3846f };
		q.scale = camera.scale * radius * gCfg._1_unitSizef;
		q.radians = 0;
		q.colorplus = 1;
		q.color = { 255, 255, 255, 255 };
		return q;
	}



	void Hero::Init(XY const& pos_) {
		pos = pos_;
		radius = gCfg.unitSizef;
	}

	xx::Task<> Hero::UpdateLogic_() {
		int32_t n = 0;
		for (int32_t i = 0, e = gScene->petsPos.len; i < e; ++i) {
			gScene->grids.Get<Pet>().EmplaceInit(*this, i);
			if (++n == 100) {
				n = 0;
				co_yield 0;
			}
		}
		while (true) {
			co_yield 0;
		}
	}

	int Hero::Update() {
		return UpdateLogic();
	}

	void Hero::Draw() {
		BaseDraw(gRes.egg_blue);
	}

	void Pet::Init(Hero& owner_, int32_t index_) {
		owner = owner_;
		index = index_;
		radius = gCfg.unitSize_2f;
		pos = owner_.pos + gScene->petsPos[index_] * radius * 2;
	}

	int Pet::Update() {
		return 0;
	}

	void Pet::Draw() {
		BaseDraw(gRes.egg_green);
	}

	int Monster::Update() {
		return 0;
	}

	void Monster::Draw() {
		BaseDraw(gRes.egg_red);
	}

}
