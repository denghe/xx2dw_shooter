#include <pch.h>
#include <all.h>

#pragma region Bullet

xx::Task<> Bullet::MainTask() {
	XY inc{ std::cos(radians) * speed,std::sin(radians) * speed };
	for (auto e = gLooper.nowSecs + cTimeSpan; gLooper.nowSecs < e;) {

		// hit check
		auto frameNumber = gLooper.frameNumber;
		auto& ms = scene->im.Get<BigMonster>();
		for (auto& m : ms) {

			// hit big monster: check when frame index mod == ??
			//if (auto mod = int(m->radius / radius); mod <= 1 || frameNumber % mod == 0) {
			//if (m->radius / radius < 2 || frameNumber % 4 == 0) {

				if (Calc::Intersects::CircleCircle<float>(
					pos.x, pos.y, radius,
					m->pos.x, m->pos.y, m->radius)) {

					for (int i = hitBlackList.len - 1; i >= 0; --i) {
						auto& o = hitBlackList[i];
						if (!o.first) {
							hitBlackList.SwapRemoveAt(i);
						} else if (o.first.GetPointer() == m.pointer) {
							if (o.second <= frameNumber) {
								o.second = frameNumber + cHitDelayFrames;

								// hit effect
								scene->enm.Add(pos, XY{ 0, -1 }, RGBA8_Red, int(radius * 1000));
								--maxHitCount;
							}
							goto LabContinue;
						}
					}
					hitBlackList.Emplace(m, frameNumber + cHitDelayFrames);

					// hit effect
					scene->enm.Add(pos, XY{ 0, -1 }, RGBA8_Red, int(radius * 1000));
					--maxHitCount;
				}

			LabContinue:
				if (maxHitCount == 0) co_return;	// todo: fade out ?

			//}
		}

		pos += inc;
		co_yield 0;
	}
}

void Bullet::Init(ItemManagerBase* im_) {
	Init(im_, gLooper.rnd.Next<float>(-gPI, gPI));
}

void Bullet::Init(ItemManagerBase* im_, float radians_) {
	SceneItemInit(cTypeId, im_);
	radians = radians_;
	scale.x = scale.y = cScale * gLooper.rnd.Next<float>(0.2f, 1.f);
	radius = cRadius * (scale.x / cScale);
	tailRatio = cRadius / radius;
	speed = gLooper.rnd.Next<float>(cSpeed.from, cSpeed.to);
	maxHitCount = gLooper.rnd.Next<int>(cMaxHitCount.from, cMaxHitCount.to);
	hitBlackList.Reserve(maxHitCount);
	mainTask = MainTask();
}

bool Bullet::Update() {
	return mainTask.Resume();
}

void Bullet::Draw(Camera const& camera) {
	{
		// tail
		auto& q = Quad::DrawOnce(gLooper.frame_trangle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = { 0.5f, 1.f };
		q.scale = XY::Make(camera.scale) * scale * XY { 1.f, speed / cSpeed.from * tailRatio };
		q.radians = radians + gPI / 2;
		q.colorplus = 1;
		q.color = RGBA8_White;
		q.color.a = 127;
	}
	{
		// body
		auto& q = Quad::DrawOnce(gLooper.frame_circle);
		q.pos = camera.ToGLPos(pos);
		q.anchor = cAnchor;
		q.scale = XY::Make(camera.scale) * scale;
		q.radians = radians;
		q.colorplus = 1;
		q.color = RGBA8_White;
	}
}

#pragma endregion

#pragma region BigMonster

void BigMonster::Init(ItemManagerBase* im_) {
	SceneItemInit(cTypeId, im_);
	radius = cRadius;
	scale.x = scale.y = cScale;
	pos = { 300, 0 };
}

bool BigMonster::Update() {
	return false;
}

void BigMonster::Draw(Camera const& camera) {
	auto& q = Quad::DrawOnce(gLooper.frame_circle);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_Blue;
}

#pragma endregion

#pragma region SceneTest1

void SceneTest1::Init() {

	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"zoom in", [&]() {
		camera.IncreaseScale(0.1f, 5);
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"zoom out", [&]() {
		camera.DecreaseScale(0.1f, 0.1f);
		});

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1", [&]() {
			numBulletGenerateByEveryFrame = 1;
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m + XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+10", [&]() {
			numBulletGenerateByEveryFrame = 10;
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 50 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+100", [&]() {
			numBulletGenerateByEveryFrame = 100;
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy4m - XY{ 0, 150 }, gDesign.xy4a, gLooper.s9cfg_btn, U"+1000", [&]() {
			numBulletGenerateByEveryFrame = 1000;
		});


	camera.SetScale(1.f);

	im.Init(this);
	enm.Init(20000);

	im.Create<BigMonster>();

	tasks.Add([this]()->xx::Task<> {
		while (true) {
			for (size_t i = 0; i < numBulletGenerateByEveryFrame; i++) {
				//im.Create<Bullet>(gLooper.rnd.Next<float>(-gPI / 8, gPI / 8));
				im.Create<Bullet>(gLooper.rnd.Next<float>(-gPI, gPI));
			}
			//co_await gLooper.AsyncSleep(1);
			co_yield 0;
		}
	});
}

void SceneTest1::Update() {
	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	//// gen some bullet
	//for (size_t i = 0; i < 5; i++) {
	//	im.Create<Bullet>();
	//}

	im.Update();
	enm.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	im.DirectDraw<BigMonster>(camera);
	im.DirectDraw<Bullet>(camera);
	enm.Draw(camera);

	auto str = xx::ToString("total bullet count = ", im.GetSize(), "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
};

#pragma endregion
