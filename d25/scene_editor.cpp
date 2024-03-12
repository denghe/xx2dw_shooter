#include "pch.h"
#include "cfg.h"
#include "scene_editor.h"
#include "scene_main_menu.h"

#pragma region SceneEditor

void SceneEditor::Init() {
	gSceneEditor = this;

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	camera.SetScale(1.f);
	camera.SetOriginal({ gCfg.unitSize * 20, gCfg.unitSize * 11 });
	camera.SetMaxFrameSize({ (float)gCfg.gridCellSize, (float)gCfg.gridCellSize });

	// ...
}

void SceneEditor::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

}

void SceneEditor::Draw() {
	camera.Calc();

	for (int i = 0, ie = gLooper.mapNumRows; i < ie; ++i) {
		for (int j = 0, je = gLooper.mapNumCols; j < je; ++j) {
			if (auto idx = gLooper.mapData[i * je + j]) {
				gLooper.tiledQuads[idx].SetPosition(camera.ToGLPos(XY{ 32.f * j, 32.f * i })).Draw();
			}
		}
	}

	gLooper.DrawNode(rootNode);
}

#pragma endregion
