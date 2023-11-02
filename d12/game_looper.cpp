#include <pch.h>

xx::Task<> GameLooper::MainTask() {
#ifdef __EMSCRIPTEN__
	auto frameLaser = Frame::Create( co_await AsyncLoadTextureFromUrl("res/laser.png") );
#else
	auto frameLaser = LoadFrame("res/laser.png");
#endif
	rq.Emplace()->SetFrame(frameLaser).SetScale({ 2,1 });
	while (true) {
		for (float x = 0; x < 1; x += 0.1f) {
			rq->SetTexOffsetXPercentage(x);
			co_yield 0;
		}
	}
	co_return;
}

void GameLooper::Update() {
	if (rq) {
		rq->SetTo(mouse.pos);
	}
}

void GameLooper::Draw() {
	if (rq) {
		rq->Draw();
	}
}
