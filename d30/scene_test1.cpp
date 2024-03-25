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


		//Rnd rnd;
		//int64_t counter{};
		//xx::BlockList<xx::Task<>> tasks;
		//auto secs = xx::NowEpochSeconds();
		//for (size_t i = 0; i < 1000000; i++) {
		//	tasks.Emplace(([](Rnd& rnd, int64_t& counter)->xx::Task<> {
		//		for (int i = 0, e = rnd.Next(100, 1000);
		//			i < e;
		//			i++) {
		//			++counter;
		//			co_yield 0;
		//		}
		//		})(rnd, counter));
		//}
		//do {
		//	tasks.Foreach([](auto& o)->xx::ForeachResult {
		//	//tasks.ForeachLink([](auto& o)->xx::ForeachResult {
		//		return o.Resume() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
		//	});
		//} while (tasks.Count());

		//xx::CoutN(xx::NowEpochSeconds(secs));
		//xx::CoutN(counter);
	}

	void Scene::BeforeUpdate() {
		//// scale control
		//if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		//	camera.IncreaseScale(0.1f, 5);
		//} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		//	camera.DecreaseScale(0.1f, 0.1f);
		//}
		//camera.Calc();
	}

	void Scene::Update() {
	}

	void Scene::Draw() {

		gLooper.DrawNode(rootNode);
	}

}

/*
result: 8.8 ~ 9.x secs

		Rnd rnd;
		int64_t counter{};
		std::list<xx::Task<>> tasks;
		auto secs = xx::NowEpochSeconds();
		for (size_t i = 0; i < 1000000; i++) {
			tasks.emplace_back(([](Rnd& rnd, int64_t& counter)->xx::Task<> {
				for (int i = 0, e = rnd.Next(100, 1000);
					i < e;
					i++) {
					++counter;
					co_yield 0;
				}
				})(rnd, counter));
		}
		do {
			for (auto iter = tasks.begin(); iter != tasks.end(); ) {
				if (iter->Resume()) {
					iter = tasks.erase(iter);
				} else {
					++iter;
				}
			}
		} while (!tasks.empty());

		xx::CoutN(xx::NowEpochSeconds(secs));
		xx::CoutN(counter);

result: 5.9 ~ 6.3 secs

		Rnd rnd;
		int64_t counter{};
		xx::Listi32<xx::Task<>> tasks;
		auto secs = xx::NowEpochSeconds();
		for (size_t i = 0; i < 1000000; i++) {
			tasks.Emplace(([](Rnd& rnd, int64_t& counter)->xx::Task<> {
				for (int i = 0, e = rnd.Next(100, 1000); i < e; i++) {
					++counter;
					co_yield 0;
				}
				})(rnd, counter));
		}
		do {
			for (int i = tasks.len - 1; i >= 0; --i) {
				if (tasks[i].Resume()) {
					tasks.SwapRemoveAt(i);
				}
			}
		} while (!tasks.Empty());

		xx::CoutN(xx::NowEpochSeconds(secs));
		xx::CoutN(counter);


result: 7.5 ~ 7.8 secs

		Rnd rnd;
		int64_t counter{};
		xx::BlockList<xx::Task<>> tasks;
		auto secs = xx::NowEpochSeconds();
		for (size_t i = 0; i < 1000000; i++) {
			tasks.Emplace(([](Rnd& rnd, int64_t& counter)->xx::Task<> {
				for (int i = 0, e = rnd.Next(100, 1000);
					i < e;
					i++) {
					++counter;
					co_yield 0;
				}
			})(rnd, counter));
		}
		do {
			tasks.ForeachEx([](auto& o)->xx::ForeachResult {
				return o.Resume() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
			});
		} while (tasks.Count());

		xx::CoutN(xx::NowEpochSeconds(secs));
		xx::CoutN(counter);
*/
