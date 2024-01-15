#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m, gDesign.xy8a, gLooper.s9cfg_btn, U"Dump Env Info", [&]() {
		env.DumpInfo();
	});

	env.Init();

	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal({ 500, 500 });
}

void ScenePlay::Update() {
	env.Update();
}

void ScenePlay::Draw() {
	camera.Calc();
	env.Draw(camera);

	gLooper.DrawNode(rootNode);
};
