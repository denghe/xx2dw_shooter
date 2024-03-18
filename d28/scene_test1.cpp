#include "pch.h"
#include "scene_test1.h"
#include "scene_main_menu.h"

void SceneTest1::Init() {
	gSceneTest1 = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	//camera.SetScale(2.f);
	//camera.SetOriginal({ gCfg.unitSize * map->width / 2, gCfg.unitSize * map->height / 2 });
	//camera.SetMaxFrameSize({ (float)gCfg.unitSize, (float)gCfg.unitSize });
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

}

void SceneTest1::Draw() {
	camera.Calc();

	//auto str = xx::ToString("total item count = ", grids.Count());// , "  total blood text count = ", enm.ens.Count());
	//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}
