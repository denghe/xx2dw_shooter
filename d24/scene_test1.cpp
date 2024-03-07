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

void BigMonster::Save(xx::Data& d) {
	d.WriteBuf(this, sizeof(BigMonster));
}

void BigMonster::Load(xx::Data& d) {
	memcpy(this, d.ReadBuf(sizeof(BigMonster)), sizeof(BigMonster));
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
	speed = rnd.Next<float>(cSpeed.from, cSpeed.to);
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
	for (e = scene->frameNumber + int(cTimeSpan / gDesign.frameDelay); e > scene->frameNumber;) {

		// hit check	( lambda return false mean foreach next m )
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

				if (o.iv == m.iv) {
					// not timeout ?
					if (o.value > scene->frameNumber) return false;

					// renew timestamp
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

// placement new logic
void Bullet::Load(xx::Data& d) {
	auto siz = _offsetof(Bullet, hitBlackList);
	memcpy(this, d.ReadBuf(siz), siz);

	decltype(hitBlackList.len) len;
	d.Read(len);
	new (&hitBlackList) decltype(hitBlackList)();
	if (len) {
		hitBlackList.Reserve(len);
		hitBlackList.len = len;
		siz = len * sizeof(PointerInt);
		memcpy(hitBlackList.buf, d.ReadBuf(siz), siz);
	}
}

void Bullet::Save(xx::Data& d) {
	auto siz = _offsetof(Bullet, hitBlackList);
	d.WriteBuf(this, siz);
	d.Write(hitBlackList.len);
	if (hitBlackList.len) {
		d.WriteBuf(hitBlackList.buf, hitBlackList.len * sizeof(PointerInt));
	}
}

#pragma endregion

#pragma region SceneTest1

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

	Make<BigMonster>().Init({ 300, 0 });
	enm.Init(20000);

	tasks.Add([this]()->xx::Task<> {
		while (true) {
			for (size_t i = 0; i < numBulletGenerateByEveryFrame; i++) {
				Make<Bullet>().Init();
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

// unsafe
void SceneTest1::Save() {
	sd.Clear();
	auto secs = xx::NowEpochSeconds();

	// backup Scene memory begin ~ end
	sd.WriteBuf((char*)this + _offsetof(SceneTest1, camera), 
		_offsetof(SceneTest1, bullets) - _offsetof(SceneTest1, camera));

	// backup bullets numbers memory
	sd.WriteBuf((char*)&bullets + _offsetof(decltype(bullets), cap),
		_offsetof(decltype(bullets), version) - _offsetof(decltype(bullets), cap) + sizeof(bullets.version));

	// backup bullets buf
	for (int i = 0, e = bullets.len; i < e; ++i) {
		auto& node = bullets.buf[i];
		sd.WriteBuf(&node, 12);		// next, prev, version : [u]int32
		if (node.version == 0) continue;
		node.value.Save(sd);
	}

	// todo: backup  bigMonsters

	enm.Save(sd);

	xx::CoutN("Save() seconds = ", xx::NowEpochSeconds(secs), " data len = ", sd.len);
}

// unsafe
void SceneTest1::Load() {
	if (!sd) return;
	sd.offset = 0;
	auto secs = xx::NowEpochSeconds();

	// restore pod memory begin ~ end
	auto siz = _offsetof(SceneTest1, bullets) - _offsetof(SceneTest1, camera);
	memcpy(&camera, sd.ReadBuf(siz), siz);

	// restore bullets numbers memory
	bullets.Clear<true>();
	siz = _offsetof(decltype(bullets), version) - _offsetof(decltype(bullets), cap) + sizeof(bullets.version);
	memcpy((char*)&bullets + _offsetof(decltype(bullets), cap), sd.ReadBuf(siz), siz);

	// restore bullets buf
	bullets.buf = (decltype(bullets.buf))malloc(sizeof(decltype(bullets)::Node) * bullets.cap);
	for (int i = 0, e = bullets.len; i < e; ++i) {
		auto& node = bullets.buf[i];
		memcpy(&node, sd.ReadBuf(12), 12);		// next, prev, version : [u]int32
		if (node.version == 0) continue;
		node.value.Load(sd);
	}

	// todo: restore  bigMonsters 

	enm.Load(sd);

	assert(!sd.HasLeft());
	xx::CoutN("Load() seconds = ", xx::NowEpochSeconds(secs), " data len = ", sd.len);
}

#pragma endregion
