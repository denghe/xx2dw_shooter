﻿#include "pch.h"
#include "scene_test3.h"
#include "scene_main_menu.h"
#include "xx_blocklink.h"

namespace Test3 {
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


		for (int j = 0; j < gCfg.petIndexRotateStep; ++j) {
			auto jinc = g2PI / gCfg.petIndexRotateStep * j;
			auto& petsPos = petsPoss.Emplace();

			float a{ 1 }, b{ 0.15f }, rot{}, s{ 1 };
			auto n = gCfg.numMaxPets;
			do {
				auto r = a + b * rot;
				auto drot = (-r + std::sqrt(r * r + 2 * b * s)) / b;
				rot += drot;
				auto r_ = a + b * rot;
				XY p{ r_ * std::cos(rot + jinc), r_ * std::sin(rot + jinc) };
				petsPos.Emplace(p);
			} while (--n > 0);
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
#if 1
		int32_t rowFrom, rowTo, colFrom, colTo;
		camera.FillRowColIdxRange(gCfg.numGridRows, gCfg.numGridCols, gCfg.unitSizei,
			rowFrom, rowTo, colFrom, colTo);

		for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
			for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
				grids.Foreach([&]<typename T>(SGS::SG<T>&grid) {
					auto idx = grid.CrIdxToCIdx({ colIdx, rowIdx });
					grid.ForeachCell(idx, [&](T& o) {
						bases.Emplace(&o);
					});
				});
			}
		}
#else
		grids.Foreach([&]<typename T>(SGS::SG<T>&grid) {
			grid.Foreach([&](auto& o) {
				if (camera.InArea(o.pos)) {
					bases.Emplace(&o);
				}
			});
		});
#endif
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
		int32_t n{};
		float nn{ 1 };
		for (int32_t i = 0; i < gCfg.numMaxPets; ++i) {
			gScene->grids.Get<Pet>().EmplaceInit(*this, i);
			if (++n == (int32_t)nn) {
				nn += 0.01f;
				n = 0;
				co_yield 0;
			}
		}

		while (true) {
			co_yield 0;
		}
	}

	int Hero::Update() {
		petPosIndex += 1;
		if (auto max = (float)gScene->petsPoss.len; petPosIndex >= max) {
			petPosIndex -= max;
		}
		return UpdateLogic();
	}

	void Hero::Draw() {
		BaseDraw(gRes.egg_blue);
	}

	void Pet::Init(Hero& owner_, int32_t petIndex_) {
		ownerWeak = owner_;
		petIndex = petIndex_;
		radius = gCfg.unitSize_2f;
		pos = owner_.pos + gScene->petsPoss[(int32_t)owner_.petPosIndex][petIndex_] * radius * 1.5;
	}

	int Pet::Update() {
		if (ownerWeak) {
			auto& owner = ownerWeak();
			pos = owner.pos + gScene->petsPoss[(int32_t)owner.petPosIndex][petIndex] * radius * 1.5;
			gScene->grids.Update(*this);
		}
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