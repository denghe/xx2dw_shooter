#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy7m, gDesign.xy7a, gLooper.s9cfg_btn, U"Back To Menu", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy8m, gDesign.xy8a, gLooper.s9cfg_btn, U"Dump Env Info", [&]() {
		//env.im.DumpInfo();
		xx::CoutN("effect number's count = ", enm.ens.Count());
	});

	env.Init();
	//enm.Init(100000);

	camera.SetMaxFrameSize({ 50, 50 });
	camera.SetOriginal(gLooper.windowSize_2);
}

void ScenePlay::Update() {
	//for (size_t i = 0; i < 1000; i++) {
	//	auto x = gLooper.rnd.Next<float>(0, gLooper.windowSize.x);
	//	auto y = gLooper.rnd.Next<float>(0, gLooper.windowSize.y);
	//	auto dx = gLooper.rnd.Next<float>(0, gLooper.windowSize.x);
	//	auto dy = gLooper.rnd.Next<float>(0, gLooper.windowSize.y);
	//	enm.Add({ x, y }, { dx - x, dy - y }, { 255,0,0,255 }, gLooper.rnd.Next<int>(1, 99999));
	//}

	env.Update();
	//enm.Update();
}

void ScenePlay::Draw() {
	camera.Calc();
	env.Draw(camera);
	//enm.Draw(camera);

	gLooper.DrawNode(rootNode);
};
