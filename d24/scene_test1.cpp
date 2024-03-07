#include <pch.h>
#include <all.h>

#pragma region BigMonster

void BigMonster::Init(XY const& pos_) {
	radius = cRadius;
	scale.x = scale.y = cScale;
	pos = pos_;
}

bool BigMonster::Update() {
	return false;
}

void BigMonster::Draw() {
	auto& camera = SceneTest1::instance->camera;
	auto& q = Quad::DrawOnce(gLooper.frame_circle);
	q.pos = camera.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(camera.scale) * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_Blue;
}

#pragma endregion

#pragma region Bullet

void Bullet::Init() {
	Init(SceneTest1::instance->rnd.Next<float>(-gPI, gPI));
}

void Bullet::Init(float radians_) {
	auto& rnd = SceneTest1::instance->rnd;
	radians = radians_;
	scale.x = scale.y = cScale * rnd.Next<float>(0.2f, 1.f);
	radius = cRadius * (scale.x / cScale);
	tailRatio = cRadius / radius;
	auto speed = rnd.Next<float>(cSpeed.from, cSpeed.to);
	inc.x = std::cos(radians) * speed;
	inc.y = std::sin(radians) * speed;
	maxHitCount = rnd.Next<int>(cMaxHitCount.from, cMaxHitCount.to);
	hitBlackList.Reserve(maxHitCount);
}

bool Bullet::Update() {
	return (lineNumber = UpdateCore()) == 0;
}

int Bullet::UpdateCore() {
	auto scene = SceneTest1::instance;
	COR_BEGIN
	for (e = (float)gLooper.nowSecs + cTimeSpan; gLooper.nowSecs < e;) {

		// hit check
		scene->bigMonsters.Foreach<false>([&](BigMonster& m) {
			// intersects ?
			if (!Calc::Intersects::CircleCircle<float>(
				pos.x, pos.y, radius, m.pos.x, m.pos.y, m.radius)) return false;

			// black list check
			for (int i = hitBlackList.len - 1; i >= 0; --i) {
				auto& o = hitBlackList[i];

				if (!o.Is<BigMonster>()) {
					if (/*!o.Exists2() || */o.value <= scene->frameNumber) {
						hitBlackList.SwapRemoveAt(i);
					}
					continue;
				}

				if (!o.Exists<BigMonster>()) {
					hitBlackList.SwapRemoveAt(i);
					continue;
				}

				// is same?
				if (memcmp(&o.iv, &m.iv, sizeof(o.iv)) == 0) {
					// not timeout ?
					if (o.value > scene->frameNumber) return false;

					// renew
					o.value = scene->frameNumber + cHitDelayFrames;
					goto LabHit;
				}
			}

			// add to black list 
			hitBlackList.Emplace(PointerInt{ {m.typeId, m.iv}, scene->frameNumber + cHitDelayFrames });

		LabHit:
			// hit effect
			scene->enm.Add(pos, XY{ 0, -1 }, RGBA8_Red, int(radius * 1000));
			--maxHitCount;
			return maxHitCount == 0;
		});

		if (maxHitCount == 0) COR_EXIT;		// todo: fade out ?
		pos += inc;
		COR_YIELD
	}
	COR_END
}

void Bullet::Draw() {
	auto& camera = SceneTest1::instance->camera;
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

void Bullet::Load(xx::Data& d) {
	memcpy(this, d.ReadBuf(_offsetof(Bullet, hitBlackList)), _offsetof(Bullet, hitBlackList));
	int len;
	d.Read(len);
	new (&hitBlackList) decltype(hitBlackList)();
	hitBlackList.Reserve(len);
	hitBlackList.len = len;
	auto siz = len * sizeof(PointerInt);
	memcpy(hitBlackList.buf, d.ReadBuf(siz), siz);
}

void Bullet::Save(xx::Data& d) {
	assert(_offsetof(Bullet, hitBlackList) == 0x48);
	d.WriteBuf((void*)this, _offsetof(Bullet, hitBlackList));
	d.Write(hitBlackList.len);
	d.WriteBuf(hitBlackList.buf, hitBlackList.len * sizeof(PointerInt));
}

#pragma endregion

#pragma region SceneTest1

Bullet& SceneTest1::MakeBullet() {
	auto& r = SceneTest1::instance->bullets.Emplace();
	r.typeId = Bullet::cTypeId;
	r.iv = SceneTest1::instance->bullets.Tail();
	return r;
}

BigMonster& SceneTest1::MakeBigMonster() {
	auto& r = SceneTest1::instance->bigMonsters.Emplace();
	r.typeId = BigMonster::cTypeId;
	r.iv = SceneTest1::instance->bigMonsters.Tail();
	return r;
}

void SceneTest1::Init() {
	instance = this;

	rootNode.Emplace()->Init();

	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m - XY{ 5,0 }, { 1,0 }, gLooper.s9cfg_btn, U"Save", [&]() {
		Save();
		});
	rootNode->MakeChildren<Button>()->Init(1, gDesign.xy2m + XY{ 5,0 }, { 0,0 }, gLooper.s9cfg_btn, U"Load", [&]() {
		Load();
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

	MakeBigMonster().Init({ 300, 0 });
	enm.Init(20000);

	tasks.Add([this]()->xx::Task<> {
		while (true) {
			for (size_t i = 0; i < numBulletGenerateByEveryFrame; i++) {
				MakeBullet().Init();
			}
			co_yield 0;
		}
	});
}

void SceneTest1::Update() {
	++frameNumber;

	// scale control
	if (gLooper.KeyDownDelay(KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}
	camera.Calc();

	bigMonsters.Foreach([](BigMonster& o) {
		return o.Update();
		});
	bullets.Foreach([](Bullet& o) {
		return o.Update();
		});
	enm.Update();
}

void SceneTest1::Draw() {
	camera.Calc();

	bigMonsters.Foreach([](BigMonster& o) {
		o.Draw();
	});
	bullets.Foreach([](Bullet& o) {
		o.Draw();
	});
	enm.Draw(camera);

	auto str = xx::ToString("total bullet count = ", bullets.Count(), "  total blood text count = ", enm.ens.Count());
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, RGBA8_Green, { 0.5f, 1 });

	gLooper.DrawNode(rootNode);
}

void SceneTest1::Save() {
	// todo
	savedData.Clear();
	auto secs = xx::NowEpochSeconds();
	savedData.WriteBuf((char*)this + _offsetof(SceneTest1, camera), _offsetof(SceneTest1, bullets) - _offsetof(SceneTest1, camera));
	savedData.Write(bullets.Count());
	// todo: backup bullets all memory?
	//bullets.Foreach([this](Bullet& o) {
	//	o.Save(savedData);
	//});
	xx::CoutN("Save() seconds = ", xx::NowEpochSeconds(secs), " data len = ", savedData.len);
}

void SceneTest1::Load() {
	if (!savedData) return;
	savedData.offset = 0;
	auto secs = xx::NowEpochSeconds();
	auto siz = _offsetof(SceneTest1, bullets) - _offsetof(SceneTest1, camera);
	memcpy((char*)this + _offsetof(SceneTest1, camera), savedData.ReadBuf(siz), siz);
	int32_t len;
	savedData.Read(len);
	bullets.Clear<true, true>();
	//bullets.Reserve(len);
	//bullets.AddCore()
	//bullets.Foreach([this](Bullet& o) {
	//	o.Save(savedData);
	//});

	xx::CoutN("Load() seconds = ", xx::NowEpochSeconds(secs), " data len = ", savedData.len);
}


#pragma endregion
