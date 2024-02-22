#include <pch.h>
#include <all.h>

#pragma region BagCell

void BagCell::Update(Bag* owner) {

}

void BagCell::Draw(Camera const& camera) {

}

#pragma endregion

#pragma region Bag

XY Bag::GetGridSize() {
	return {
		gridPadding.x * 2 + BagCell::cellMargin.x * (numCols - 1),
		gridPadding.y * 2 + BagCell::cellMargin.y * (numRows - 1)
	};
}

void Bag::Init(int numRows_, int numCols_) {
	numRows = numRows_;
	numCols = numCols_;
}

void Bag::Update() {
	// todo: mouse drag item

}

void Bag::Draw(Camera const& camera) {
	// todo: draw background grid lines ?

}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {
	camera.SetMaxFrameSize({ gCfg.cellSize, gCfg.cellSize });
	camera.SetOriginal(gCfg.gridCenterPos);
	camera.SetScale(1.f);

	bag.Init(20, 30);
}

void SceneTest1::Update() {
	camera.Calc();

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

}

void SceneTest1::Draw() {
	camera.Calc();
	bag.Draw(camera);
};

#pragma endregion
