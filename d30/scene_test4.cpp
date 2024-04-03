#include "pch.h"
#include "scene_test4.h"
#include "scene_main_menu.h"

namespace Test4 {
	void Scene::Draw() {
		MainTask();
	}

	xx::Task<> Scene::MainTask_() {
		auto& quad = Quad().SetFrame(gRes.egg_orange);

		xx::Tasks myTasks;
		for (float y = -200; y < 200; y += 10) {

			myTasks.Add([&quad, y = y]()->xx::Task<> {

				XY pos{-400, y}, inc{ 1, 0 };
				for (size_t i = 0; i < 800; i++) {
					pos += inc;
					quad.SetPosition(pos).Draw();
					co_yield 0;
				}
			});
		}

		while (myTasks()) co_yield 0;	// wait all
	}
}
