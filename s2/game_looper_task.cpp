#include "pch.h"

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();

	// preload texs
    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");
	xx_assert(tp);

	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");			// ready flag

	shooter.Emplace()->Init();	// make player char

	ready = true;
	co_return;
}

void GameLooper::Update() {
	fv.Update();
	if (!ready) return;

	shooter();
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (!ready) return;

	shooter->Draw();
	bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw

	fv.Draw(ctc72);       // draw fps at corner
}

