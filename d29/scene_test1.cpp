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

		camera.SetScale(1.f);
		camera.SetMaxFrameSize({ gCfg.unitSize, gCfg.unitSize });
		camera.SetOriginal(gCfg.mapCenterPos);

		explosionManager.Init(&frameNumber, &camera, &rnd, gRes.td_effect_1);

		gameSpeedRate = 1;

		blocks.Init(gCfg.numRows, gCfg.numCols, (int32_t)gCfg.unitSize);

		// fill avaliableBlockIndexs
		for (int32_t row = 2; row < gCfg.numRows - 5; ++row) {
			for (int32_t col = 2; col < gCfg.numCols - 2; ++col) {
				blockIndexs.Emplace(gCfg.numCols * row + col);
			}
		}

		// make walls
		walls.Emplace().Init({ { 0, 0 }, { 0, gCfg.numRows - 1 } });
		walls.Emplace().Init({ { 1, 0 }, { gCfg.numCols - 1, 0 } });
		walls.Emplace().Init({ { gCfg.numCols - 1, 0 }, { gCfg.numCols - 1, gCfg.numRows - 1 } });
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	xx::Task<> Scene::AsyncSleep(float secs) {
		for (int32_t e = frameNumber + int32_t(secs / gDesign.frameDelay); frameNumber < e;) co_yield 0;
	}

	xx::Task<> Scene::MainTask_() {
		// stage data init
		int32_t stage = 1;
		auto barBornPos = blocks.CrIdxToCenterPos(gCfg.numCols / 2, gCfg.numRows - 1);

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
				co_await AsyncSleep(0.2f);
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

		//auto str = xx::ToString("total partical count = ", em.items.len);
		//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

		gLooper.DrawNode(rootNode);
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Box::BoxInit(XY const& pos_, XY const& size_) {
		x = pos_.x;
		y = pos_.y;
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
				q.color = RGBA8_White;
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
		q.pos = camera.ToGLPos(x, y);
		q.anchor = { 0.5f, 0.5f };
		q.scale = size * s;
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;

		// todo: draw hp number ?
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	xx::Task<> Ball::MainTask_() {
		XY v{ std::cos(radians), std::sin(radians) };
		auto inc = v * speed / (float)gCfg.ballMoveStepRate;
		while (true) {
			for (int i = 0; i < gCfg.ballMoveStepRate; ++i) {
				pos += inc;
				// todo: hit check, bounce
				if (pos.x < 0 || pos.x >= gCfg.mapSize.x) co_return;
				if (pos.y < 0 || pos.y >= gCfg.mapSize.y) co_return;
			}
			co_yield 0;
		}
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
		q.color = RGBA8_White;
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

		gScene->balls.Emplace().Init({ x, y - size.y / 2 }
		, gCfg.unitSize_2
			, gScene->rnd.Next<float>(gNPI + 0.1f, -0.1f)
			, 1);

		return false;
	}

	void Bar::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_mask;
		auto& q = Quad::DrawOnce(frame);
		auto s = (1.f / gCfg.unitSize) * camera.scale;
		q.pos = camera.ToGLPos(x, y);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { size.x * s, size.y * s };
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}

}



//auto pos = gCfg.mapSize_2;
//pos.x += gLooper.rnd.Next<float>(-gLooper.windowSize_2.x + 100, gLooper.windowSize_2.x - 100);
//pos.y += gLooper.rnd.Next<float>(-gLooper.windowSize_2.y + 100, gLooper.windowSize_2.y - 100);
//float radius = gLooper.rnd.Next<float>(16, 32);
//int32_t count = gLooper.rnd.Next<int32_t>(64, 512);
//em.Add(pos, radius, count);

//co_await gLooper.AsyncSleep(0.5f);
//co_yield 0;
