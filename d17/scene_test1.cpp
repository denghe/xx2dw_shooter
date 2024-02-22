#include <pch.h>
#include <all.h>

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ gCfg.cellSize, gCfg.cellSize });
	camera.SetOriginal(gCfg.gridCenterPos);
	camera.SetScale(1.f);

	// todo
}

void SceneTest1::Update() {
	camera.Calc();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

	// todo: mouse drag item
}

void SceneTest1::Draw() {
	camera.Calc();

	// todo: draw background grid lines ?

	// todo
};

#pragma endregion
