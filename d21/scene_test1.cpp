#include <pch.h>
#include <all.h>

#pragma region Monster1

void Monster1::Init(SceneTest1* scene_, int tracksIndex_, float pointIndex_, float speed_) {
	scene = scene_;
	scale = cScale;
	radius = cRadius;
	speed = speed_;
	tracksIndex = tracksIndex_;
	pointIndex = pointIndex_;
	pos = scene->tracks[tracksIndex_][(int)pointIndex];
	radians = scene->mpc.points[(int)pointIndex].radians;
}

void Monster1::Update() {
	auto siz = (int)scene->mpc.points.size();
	pointIndex += speed;
	if (pointIndex >= siz) {
		pointIndex -= siz;
	}
	pos = scene->tracks[tracksIndex][(int)pointIndex];
	radians = scene->mpc.points[(int)pointIndex].radians;
}

void Monster1::Draw() {
	auto& c = scene->camera;
	auto& q = Quad::DrawOnce(gLooper.frames_coin_2[0]);
	q.pos = c.ToGLPos(pos);
	q.anchor = cAnchor;
	q.scale = XY::Make(c.scale) * scale;
	q.radians = radians;
	q.colorplus = 1;
	q.color = RGBA8_White;
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

	camera.SetScale(1.f);

	// curve points
	std::vector<CurvePoint> cps{ CurvePoint
		{ XY{ -300, -180} },
		{ XY{ 300, -180} },
		{ XY{ 300, 180} },
		{ XY{ -300, 180} }
	};

	// fill path
	MovePath mp;
	mp.FillCurve(true, cps);

	// path to cache
	mpc.Init(mp, trackStep);

	// cache to tracks
	tracks.Resize(trackCount);
	auto numPoints = (int)mpc.points.size();
	for (auto& track : tracks) {
		track.Resize(numPoints);
	}
	for (int i = 0; i < numPoints; ++i) {
		auto& p = mpc.points[i];
		XY step{
			std::cos(p.radians - gPI / 2) * trackMargin,
			std::sin(p.radians - gPI / 2) * trackMargin
		};
		for (int j = 0; j < trackCount; j++) {
			tracks[j][i] = p.pos + step * j;
		}
	}

	// init main task for create monsters
	tasks.Add([this]()->xx::Task<> {
		while (true) {
			// create some monsters
			if (monsters.len < 100000) {
				for (int i = 0; i < trackCount; i++) {
					for (int j = 0; j < 10; j++) {
						monsters.Emplace().Emplace<Monster1>()->Init(this, i
							, gLooper.rnd.Next<float>(0, 100)
							, gLooper.rnd.Next<float>(trackBaseSpeed, trackBaseSpeed * 5));
					}
				}
			}
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

	for (auto& o : monsters) {
		o->Update();
	}
}

void SceneTest1::Draw() {
	camera.Calc();

	// todo: draw xxxx
	LineStrip ls;
	auto siz = (int)mpc.points.size();
	for (int i = 0; i < siz; i += trackDrawStep) {
		auto& p = mpc.points[i];
		ls.Clear();
		ls.points.emplace_back(camera.ToGLPos(tracks[0][i]));
		ls.points.emplace_back(camera.ToGLPos(tracks[trackCount - 1][i]));
		ls.Draw();
	}

	for (auto& o : monsters) {
		o->Draw();
	}

	auto str = xx::ToString("zoom: Z / X. monster count = ", monsters.len);
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, str, RGBA8_Green, { 0.5f, 1 });
	gLooper.DrawNode(rootNode);
};

#pragma endregion
