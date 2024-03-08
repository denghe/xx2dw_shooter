﻿#include <pch.h>
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

	// init
	grid.Init(2, 3, 10);
	xx::CoutN("************************ 1");

	auto& foo1 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("************************ 2");

	auto& foo2 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("************************ 3");

	auto& foo3 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("foo3: ", foo3);
	xx::CoutN("************************ 4");

	//grid.Update(foo, { 15,15 });
	//xx::CoutN(foo.idx, "  ", foo.cidx, "  ", foo.pos);

	grid.Update(foo2, { 15,15 });
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("foo3: ", foo3);
	xx::CoutN("************************ 5");

	//grid.Foreach([](Foo& f)->void {
	//	xx::CoutN(f);
	//});


	grids.InitAll(1, 1, 1);

	auto& b = grids.MakeInit<B>({});
	xx::CoutN("b = ", b);
	auto p = b.ToGridsWeak();
	xx::CoutN("p = ", p);
	xx::CoutN(grids.Exists(p));
	grids.Remove(p);
	xx::CoutN(grids.Exists(p));
	xx::CoutN("b = ", b);



	Grid<D> dGrid(10000, 10000, 32);
	dGrid.Reserve(110000);
	for (size_t i = 0; i < 100000; i++) {
		auto x = gLooper.rnd.Next(dGrid.numCols - 1) * 16.f;
		auto y = gLooper.rnd.Next(dGrid.numRows - 1) * 16.f;
		dGrid.Make({ x, y });
	}

	dGrid.BufForeach([&](D& o)->GridForeachResult {
		return o.idx < 99999 ? GridForeachResult::RemoveAndContinue : GridForeachResult::Break;
	});

	auto secs = xx::NowEpochSeconds();
	int counter = 0;
	dGrid.BufForeach([&](D& o)->void {
		counter += o.val;
	});
	xx::CoutN(xx::NowEpochSeconds(secs), " counter = ", counter);
	counter = 0;
	for (int i = 0, e = dGrid.len; i < e; ++i) {
		if (auto& o = dGrid.buf[i]; o.version > 0) {
			counter += o.val;
		}
	}
	xx::CoutN(xx::NowEpochSeconds(secs), " counter = ", counter);



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
