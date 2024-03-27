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
