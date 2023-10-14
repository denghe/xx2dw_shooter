#include "pch.h"

GameLooper gLooper;
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx2dw_d2";
	gLooper.Init();
	gLooper.Run();
}

xx::Task<> GameLooper::MainTask() {
	rq.Emplace()->SetFrame(LoadSharedFrame("res/laser.png")).SetScale({ 2,1 });
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
	rq->Draw();
}
