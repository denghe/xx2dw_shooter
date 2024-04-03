#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_pivot_editor.h"

namespace Scene_PivotEditor {
	void Scene::Init() {
		gScene = this;

		// init camera
		//camera.SetMaxFrameSize(gCfg.unitSizef);
		//camera.SetOriginal(gCfg.mapSize_2);
		camera.SetScale(0.5f);
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
	}

	void Scene::Draw() {
		camera.Calc();

	}

}
