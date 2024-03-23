#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"

namespace Test1 {

	void Scene::ShuffleBlockIndexs() {
		auto len = blockIndexs.len;
		if (!len) return;
		auto buf = blockIndexs.buf;
		for (int32_t i = 0, tar = 1; ++i != len; ++tar) {
			if (int32_t offset = rnd.Next(tar); offset != tar) {
				std::swap(buf[i], buf[offset]);
			}
		}
	}

	void Scene::ShowText(XY const& pos, std::string_view const& txt) {
		gLooper.ctcDefault.Draw(pos, txt, RGBA8_Green, { 0.5f, 0.5f });
	}

	void Scene::Init() {
		gScene = this;

		rootNode.Emplace()->Init();
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
			gLooper.DelaySwitchTo<SceneMainMenu>();
			});
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ -300, 0 }, gDesign.xy8a, gLooper.s9cfg, U"pause", [&]() {
			gameSpeedRate = 0;
			});
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ -150, 0 }, gDesign.xy8a, gLooper.s9cfg, U"speed: 1", [&]() {
			gameSpeedRate = 1;
			});
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m, gDesign.xy8a, gLooper.s9cfg, U"speed: 5", [&]() {
			gameSpeedRate = 5;
			});
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ 150, 0 }, gDesign.xy8a, gLooper.s9cfg, U"speed: 20", [&]() {
			gameSpeedRate = 20;
			});
		rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m + XY{ 300, 0 }, gDesign.xy8a, gLooper.s9cfg, U"speed: 50", [&]() {
			gameSpeedRate = 50;
			});


		camera.SetScale(1.f);
		camera.SetMaxFrameSize({ gCfg.unitSize, gCfg.unitSize });
		camera.SetOriginal(gCfg.mapCenterPos - XY{ 0, gCfg.unitSize });

		explosionManager.Init(&frameNumber, &camera, &rnd, gRes.td_effect_1);

		gameSpeedRate = gCfg.defaultGameSpeedRate;

		blocks.Init(gCfg.numRows, gCfg.numCols, (int32_t)gCfg.unitSize);

		// fill avaliableBlockIndexs
		for (int32_t row = 2; row < gCfg.numRows - 5; ++row) {
			for (int32_t col = 2; col < gCfg.numCols - 2; ++col) {
				blockIndexs.Emplace(gCfg.numCols * row + col);
			}
		}

		// make walls
		walls.Emplace().Init({ { 0, 0 }, { 0, gCfg.numRows - 2 } });
		walls.Emplace().Init({ { 1, 0 }, { gCfg.numCols - 2, 0 } });
		walls.Emplace().Init({ { gCfg.numCols - 1, 0 }, { gCfg.numCols - 1, gCfg.numRows - 2 } });
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	xx::Task<> Scene::AsyncSleep(float secs) {
		for (int32_t e = frameNumber + int32_t(secs / gDesign.frameDelay); frameNumber < e;) co_yield 0;
	}

	xx::Task<> Scene::MainTask_() {
		// stage data init
		int32_t stage = 50;

		auto barBornPos = blocks.CrIdxToCenterPos(gCfg.numCols / 2, gCfg.numRows - 2);

		// begin loop
		while (true) {
			// todo: read config by stage?
			// show Level xxxx 1 secs
			{
				auto holder = globalEffects.HolderEmplace();
				auto str = xx::ToString("Stage ", stage);
				holder().draw = [&] {
					ShowText({}, str);
					};
				co_await AsyncSleep(1);
			}
			// step by step create some blocks
			ShuffleBlockIndexs();
			int32_t e = std::min(stage, blockIndexs.len);
			for (int32_t i = 0; i < e; ++i) {
				auto cidx = blockIndexs[i];
				auto pos = blocks.CIdxToCenterPos(cidx);
				blocks.EmplaceInit(pos, gCfg.unitXYSize, 100);
				co_await AsyncSleep(0.05f);
			}
			// todo: create bar with fade in effect
			co_await AsyncSleep(0.5f);
			bar.Emplace()->Init(barBornPos, XY{200, 20}, 3000);

			// show 3, 2, 1
			{
				co_await AsyncSleep(0.5f);
				auto holder = globalEffects.HolderEmplace();
				int32_t n{ 3 };
				holder().draw = [&] {
					ShowText(camera.ToGLPos(barBornPos) - XY{ 0, -25 }, xx::ToString(n));
					};
				for (; n > 0; --n) {
					co_await AsyncSleep(1);
				}
			}
			// wait blocks empty
			while (blocks.Count()) {

				// mouse control bar. bar auto shoot ball
				bar->Update();

				// GM command
				if (gLooper.mouse.Pressed(0)) {
					blocks.Clear();
				}

				co_yield 0;
			}
			// show Clear 1 secs
			{
				auto holder = globalEffects.HolderEmplace();
				holder().draw = [&] {
					ShowText({}, "Clear!");
				};
				co_await AsyncSleep(1);
			}
			// todo: show rank / score report ?
			// todo: bar fade out?
			bar.Reset();
			// next stage or quit ?
			++stage;
			co_yield 0;
		}
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

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

			MainTask();

			blocks.Foreach([](Block& o)->xx::ForeachResult {
				if (o.Update()) return xx::ForeachResult::RemoveAndContinue;
				return xx::ForeachResult::Continue;
			});

			balls.Foreach([](Ball& o)->xx::ForeachResult {
				if (o.Update()) return xx::ForeachResult::RemoveAndContinue;
				return xx::ForeachResult::Continue;
			});

			explosionManager.Update();
		}
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Scene::Draw() {

		for (auto& o : walls) {
			o.Draw();
		}

		if (bar) {
			bar->Draw();
		}

		blocks.Foreach([](Block& o) {
			o.Draw();
			});

		balls.Foreach([](Ball& o) {
			o.Draw();
			});

		explosionManager.Draw();

		// draw order by z
		if (int32_t count = globalEffects.Count()) {
			zdraws.Reserve(count);
			globalEffects.Foreach([&](GlobalEffect& o) {
				zdraws.Emplace(ZDraw{ o.z, o.draw });
				});
			std::sort(zdraws.buf, zdraws.buf + zdraws.len, [](auto const& a, auto const& b) {
				return a.z < b.z;
				});
			for (auto& o : zdraws) {
				o.draw();
			}
			zdraws.Clear();
		}

		auto str = xx::ToString("ball count = ", balls.Count(), " particle count = ", explosionManager.items.Count());
		ShowText({ 0, gLooper.windowSize_2.y - 55 }, str);

		gLooper.DrawNode(rootNode);
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Box::BoxInit(XY const& pos_, XY const& size_) {
		pos = pos_;
		size = size_;
		auto tmp = size / 2;
		xy.from = pos_ - tmp;
		xy.to = pos_ + tmp;
	}

	void Wall::Init(xx::FromTo<Vec2<>> const& cidx_) {
		cidx = cidx_;
		assert(cidx.to.x >= cidx.from.x);
		assert(cidx.to.y >= cidx.from.y);
		xy.from = cidx_.from.As<float>() * gCfg.unitSize;
		xy.to = cidx_.to.As<float>() * gCfg.unitSize + gCfg.unitSize;
	}

	void Wall::Draw() {
		auto& eb = EngineBase1::Instance();
		auto& frame = *gRes.td_shape_block;
		auto& camera = gScene->camera;
		auto count = (cidx.to.x - cidx.from.x + 1) * (cidx.to.y - cidx.from.y + 1);
		auto qd = eb.ShaderBegin(eb.shaderQuadInstance).Draw(frame.tex->GetValue(), count);
		int32_t i{};
		for (int32_t col = cidx.from.x; col <= cidx.to.x; ++col) {
			for (int32_t row = cidx.from.y; row <= cidx.to.y; ++row) {
				auto& q = qd[i++];
				q.pos = camera.ToGLPos(XY{ col * gCfg.unitSize, row * gCfg.unitSize });
				q.anchor = { 0, 1 };
				q.scale = { camera.scale, camera.scale };
				q.radians = 0;
				q.colorplus = 1;
				q.color = { 255,255,255,127 };
				q.texRect.data = frame.textureRect.data;
			}
		}
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Block::Init(XY const& pos_, XY const& size_, int32_t hp_) {
		BoxInit(pos_, size_);
		hp = hp_;
	}

	bool Block::Update() {
		return false;
	}

	void Block::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_rect;
		auto& q = Quad::DrawOnce(frame);
		auto s = (1.f / gCfg.unitSize) * camera.scale;
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = size * s;
		q.radians = 0;
		q.colorplus = 1;
		q.color = { 255,255,255,127 };

		// todo: draw hp number ?
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	xx::Task<> Ball::MainTask_() {
		XY v{ std::cos(radians), std::sin(radians) };
		auto inc = v * speed;
		while (true) {
			for (int i = 0; i < gCfg.updateMultipleTimes; ++i) {

				auto newPos = pos + inc;

				// bounce with block
				gScene->blocks.Foreach9(newPos.x, newPos.y, [&](Block& o)->xx::ForeachResult {
					if (TranslateControl::BounceCircleIfIntersectsBox(o.xy, radius, speed, inc, newPos)) {
						// hit	// todo: damage set
						if (--o.hp <= 0) {
							gScene->explosionManager.Add(o.pos, gCfg.unitSize, 5000, 3, true);
							return xx::ForeachResult::RemoveAndContinue;
						} else {
							gScene->explosionManager.Add(o.pos, gCfg.unitSize_2, 8, 0.25f);
						}
					}
					return xx::ForeachResult::Continue;
				});

				// bounce with bar	// todo
				if (gScene->bar) {
					(void)TranslateControl::BounceCircleIfIntersectsBox(gScene->bar->xy, radius, speed, inc, newPos);
				}

				// bounce with wall
				for (auto& wall : gScene->walls) {
					(void)TranslateControl::BounceCircleIfIntersectsBox(wall.xy, radius, speed, inc, newPos);
				}

				if (newPos.x < gCfg.unitSize_2 || newPos.x >= gCfg.mapSize.x - gCfg.unitSize_2
					|| newPos.y < gCfg.unitSize_2 || newPos.y >= gCfg.mapSize.y - gCfg.unitSize_2) goto LabEnd;

				pos = newPos;
			}
			co_yield 0;
		}
	LabEnd:;
		// todo: ball fail logic ?
	}

	void Ball::Init(XY const& pos_, float radius_, float radians_, float speed_) {
		pos = pos_;
		radius = radius_;
		radians = radians_;
		speed = speed_;
	}

	bool Ball::Update() {
		return MainTask.Resume();
	}

	void Ball::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_circle;
		auto& q = Quad::DrawOnce(frame);
		auto s = camera.scale * (radius / gCfg.unitSize_2);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { s, s };
		q.radians = 0;
		q.colorplus = 1;
		q.color = {255,255,255,127};
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Bar::Init(XY const& pos_, XY const& size_, float speed_) {
		BoxInit(pos_, size_);
		speed = speed_;
	}

	bool Bar::Update() {
		// todo: random shoot, mouse control
		//if (gScene->frameNumber % 60 != 0) return false;

		for (size_t i = 0; i < 10; i++) {

			gScene->balls.Emplace().Init({ x, y - size.y / 2 - gCfg.unitSize_2 }
			, gCfg.unitSize_2
				, gScene->rnd.Next<float>(gNPI + 0.1f, -0.1f)
				, gCfg.ballSpeed);
		}

		return false;
	}

	void Bar::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_mask;
		auto& q = Quad::DrawOnce(frame);
		auto s = (1.f / gCfg.unitSize) * camera.scale;
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = size * s;
		q.radians = 0;
		q.colorplus = 1;
		q.color = { 255,255,255,127 };
	}

}
