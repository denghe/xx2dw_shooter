#include <pch.h>
#include <all.h>

void ScenePlay::Init() {
	light.Init();
	grass.Init();

	rootNode.Emplace()->Init();
	rootNode->MakeChildren<Button>()->Init(1, { 0, -200 }, { 0.5f, 0.5f }, gLooper.s9cfg_btn, U"Exit Game", [&]() {
		gLooper.DelaySwitchTo<SceneMainMenu>();
	});

	auto wh_2 = gLooper.windowSize_2;
	for (size_t i = 0; i < 1000; i++) {
		auto&& h = heroes.Emplace().Emplace();
		h->pos = { gLooper.rnd.Next<float>(-wh_2.x + 50, wh_2.x - 50)
			, gLooper.rnd.Next<float>(-wh_2.y + 10, wh_2.y - 100) };
		h->Init(gLooper.rnd.Next<float>(1,3), gLooper.rnd.Next<int>(0, 100), 100);
	}

	symbolQuad.SetScale(16).SetColor({ 255,255,255,85 }).SetFrame(gLooper.frames_symbol_2[0]);
	tasks.Add([this]()->xx::Task<> {
		float frameIndex{}, radians{};
		while (true) {
			symbolQuad.SetRotate(radians).SetFrame(gLooper.frames_symbol_2[(size_t)frameIndex]);
			co_yield 0;
			frameIndex += 0.2f;
			if (frameIndex >= 4) {
				frameIndex -= 4;
			}
			radians += 0.01f;
			if (radians >= float(M_PI * 2)) {
				radians -= float(M_PI * 2);
			}
		}
	});
}

void ScenePlay::Draw() {
	auto& eb = EngineBase1::Instance();

	grass.Draw();

	for (auto& h : heroes) {
		h->Draw();
	}

	symbolQuad.frame->tex->SetGLTexParm<GL_LINEAR>();
	light.Draw([&] {
		symbolQuad.Draw();
	});
	symbolQuad.frame->tex->SetGLTexParm<GL_NEAREST>();

	for (auto& h : heroes) {
		h->DrawHP();
	}

	gLooper.DrawNode(rootNode);
};
