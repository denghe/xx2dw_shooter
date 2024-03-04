#include <pch.h>
#include <all.h>

#pragma region SceneTest1

void SceneTest1::Init() {

	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});

	camera.SetScale(1.f);

	frame_ring = gLooper.LoadFrame("res/ring.png");

	shaderRingInstance.Init(gEngine);
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	//camera.Calc();
}

void SceneTest1::Draw() {
	camera.Calc();

#if 1
	RingInstanceData rid{};
	rid.pos = {};
	rid.radius = 200;
	rid.color = RGBA8_Green;
	for (size_t i = 0; i < 1; i++) {
		shaderRingInstance.DrawOne(rid);
	}
#else	
	Quad q;
	q.SetFrame(frame_ring);
	for (size_t i = 0; i < 1000; i++) {
		q.Draw();
	}
#endif

	gLooper.DrawNode(rootNode);
};

#pragma endregion
