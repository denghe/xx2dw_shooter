#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"

namespace Test1 {

	void Scene::Shuffle() {
		auto len = avaliableBlockIndexs.len;
		if (!len) return;
		auto buf = avaliableBlockIndexs.buf;
		for (int32_t i = 0, tar = 1; ++i != len; ++tar) {
			if (int32_t offset = rnd.Next(tar); offset != tar) {
				std::swap(buf[i], buf[offset]);
			}
		}
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

		mainTask = MainTask();

		gameSpeedRate = 1;

		// fill avaliableBlockIndexs
		auto maxRowBlocksCount = gCfg.numCols - 3 - 2;
		for (int32_t row = 2; row <= gCfg.numRows - 3; ++row) {
			for (int32_t col = 2; col <= gCfg.numCols - 3; ++col) {
				avaliableBlockIndexs.Emplace(maxRowBlocksCount * row + col);
			}
		}

		// make walls
		walls.Emplace().Init({ { 0, 0 }, { 0, gCfg.numRows - 1 } });
		walls.Emplace().Init({ { 1, 0 }, { gCfg.numCols - 1, 0 } });
		walls.Emplace().Init({ { gCfg.numCols - 1, 0 }, { gCfg.numCols - 1, gCfg.numRows - 1 } });

	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	xx::Task<> Scene::MainTask() {

		// show Level xxxx 3 secs
		{
			auto holder = globalEffects.HolderEmplace();
			auto str = xx::ToString("Stage ", 1);
			holder().draw = [&] {
				gLooper.ctcDefault.Draw({ 0, 0 }, str, RGBA8_Green, { 0.5f, 0 });
				};
			for (int32_t e = frameNumber + int32_t(3.f / gDesign.frameDelay); frameNumber < e;) co_yield 0;
		}

		// step by step create some blocks
		Shuffle();
		for (int32_t i = 0
			, e = rnd.Next<int32_t>(avaliableBlockIndexs.len / 2, avaliableBlockIndexs.len - 1)
			; i < e; ++i) {

			// todo
		}
		

		// todo: create bar with fade in effect
		// 3,2,1 bar can move with mouse. bar auto shoot ball
		// loop check if blocks empty
		// next level



		//auto pos = gCfg.mapSize_2;
		//pos.x += gLooper.rnd.Next<float>(-gLooper.windowSize_2.x + 100, gLooper.windowSize_2.x - 100);
		//pos.y += gLooper.rnd.Next<float>(-gLooper.windowSize_2.y + 100, gLooper.windowSize_2.y - 100);
		//float radius = gLooper.rnd.Next<float>(16, 32);
		//int32_t count = gLooper.rnd.Next<int32_t>(64, 512);
		//em.Add(pos, radius, count);

		//co_await gLooper.AsyncSleep(0.5f);
		co_yield 0;
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
			mainTask();

			blocks.Foreach([](Block& o)->xx::ForeachResult {
				if (o.Update()) return xx::ForeachResult::RemoveAndContinue;
				return xx::ForeachResult::Continue;
				});

			balls.Foreach([](Ball& o)->bool {
				return o.Update();
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
				zdraws.Emplace(ZDraw{ o.z, &o.draw });
				});
			std::sort(zdraws.buf, zdraws.buf + zdraws.len, [](auto const& a, auto const& b) {
				return a.z < b.z;
				});
			for (auto& o : zdraws) {
				(*o.draw)();
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
		pos = pos_;
		size = size_;
		auto tmp = size / 2;
		xy.from = pos - tmp;
		xy.to = pos + tmp;
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
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		q.scale = { size.x * s, size.y * s };
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;

		// todo: draw hp number ?
	}

	/******************************************************************************************************/
	/******************************************************************************************************/

	void Ball::Init(XY const& pos_, float radius_) {
		pos = pos_;
		radius = radius_;
		// todo: init inc
	}

	bool Ball::Update() {
		// todo: pos += inc. bounce with wall, bar logic. hit check
		return false;
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

	}

	bool Bar::Update() {
		return false;
	}

	void Bar::Draw() {
		auto& camera = gScene->camera;
		auto& frame = gRes.td_shape_mask;
		auto& q = Quad::DrawOnce(frame);
		//auto s = (1.f / gCfg.unitSize) * camera.scale;
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		//q.scale = { size.x * s, size.y * s };
		q.radians = 0;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}

}
