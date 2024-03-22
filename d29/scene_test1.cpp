#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"

namespace Test1 {

	void Scene::Shuffle() {
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

	xx::Task<> Scene::MainTask_() {

		// stage data init
		int32_t stage = 1;

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
				for (int32_t e = frameNumber + int32_t(1.f / gDesign.frameDelay); frameNumber < e;) co_yield 0;
			}

			// step by step create some blocks
			Shuffle();
			int32_t e = std::min(stage, blockIndexs.len);
			for (int32_t i = 0; i < e; ++i) {
				auto cidx = blockIndexs[i];
				auto pos = blocks.CIdxToCenterPos(cidx);
				blocks.EmplaceInit(pos, gCfg.unitXYSize, 100);
				co_yield 0;
			}

			// todo: create bar with fade in effect

			// show 3, 2, 1
			{
				auto holder = globalEffects.HolderEmplace();
				int32_t n{3};
				auto pos = blocks.CrIdxToCenterPos(gCfg.numCols / 2, gCfg.numRows - 1);
				holder().draw = [&] {
					ShowText(camera.ToGLPos(pos), xx::ToString(n));
				};
				do {
					for (int32_t e = frameNumber + int32_t(1.f / gDesign.frameDelay); frameNumber < e;) co_yield 0;
					--n;
				} while (n);
			}

			// wait blocks empty
			while (blocks.Count()) {

				// todo: mouse control bar. bar auto shoot ball
				if (gLooper.mouse.Pressed(0)) {
					blocks.Clear();	// simulate
				}
				co_yield 0;
			}

			// todo: show rank / score report ?

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
		q.pos = camera.ToGLPos(x,y);
		q.anchor = { 0.5f, 0.5f };
		q.scale = size * s;
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
		//q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 0.5f };
		//q.scale = { size.x * s, size.y * s };
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
