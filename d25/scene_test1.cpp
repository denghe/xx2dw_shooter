#include <pch.h>
#include <all.h>



#pragma region SceneTest1

void SceneTest1::Init() {
	instance = this;

	rootNode.Emplace()->Init();

	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"Save", [&]() {
	//	Save();
	//	});
	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"Load", [&]() {
	//	Load();
	//	});

	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1", [&]() {
	//		numBulletGenerateByEveryFrame = 1;
	//	});
	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+10", [&]() {
	//		numBulletGenerateByEveryFrame = 10;
	//	});
	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+100", [&]() {
	//		numBulletGenerateByEveryFrame = 100;
	//	});
	//rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1000", [&]() {
	//		numBulletGenerateByEveryFrame = 1000;
	//	});

	camera.SetScale(1.f);

	grid.Init(2, 3, 10);

	auto& foo = grid.Emplace({ 5,5 }, 3.f);
	xx::CoutN(foo.idx, "  ", foo.cidx, "  ", foo.pos);

	grid.Update(foo, { 15,15 });
	xx::CoutN(foo.idx, "  ", foo.cidx, "  ", foo.pos);


	//tasks.Add([this]()->xx::Task<> {
	//	while (true) {
	//		//for (size_t i = 0; i < numBulletGenerateByEveryFrame; i++) {
	//		//	Make<Bullet>().Init();
	//		//}
	//		co_yield 0;
	//	}
	//});
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

	//auto str = xx::ToString("total bullet count = ", bullets.Count(), "  total blood text count = ", enm.ens.Count());
	//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}


#pragma endregion
