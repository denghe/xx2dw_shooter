#include "pch.h"

int32_t main() {
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
	}, nullptr);
}
GameLooper gLooper;											// global var for easy use

void GameLooper::Init() {
    windowWidth = gDesign.width;
    windowHeight = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc72.Init();											// font init
	{
		auto tp = co_await AsyncLoadTexturePackerFromUrl("res/dungeon.blist");
		xx_assert(tp);
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "boy_2_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_1_");
		tp->GetToByPrefix(frames_heros.emplace_back(), "girl_2_");
	}
	ready = true;

	heros.EmplaceShared()->Init(0, { -30, 30 });
	heros.EmplaceShared()->Init(1, { 30, 30 });
	heros.EmplaceShared()->Init(2, { 30, -30 });
	heros.EmplaceShared()->Init(3, { -30, -30 });

	while (true) co_yield 0;								// do not quit
}

void GameLooper::Update() {
	fv.Update();
	if (KeyDownDelay(KeyboardKeys::Z, 0.02)) {				// zoom control
		camera.DecreaseScale(0.02, 0.02);
	} else if (KeyDownDelay(KeyboardKeys::X, 0.02)) {
		camera.IncreaseScale(0.02, 5);
	}
	if (!ready) return;

	for (auto& o : heros) { o->mainLogic(); }
}

void GameLooper::Draw() {
	if (ready) {
		for (auto& o : heros) { o->Draw(); }
	}
	fv.Draw(ctc72);											// show fps
}

void Hero::Init(int heroId_, XY bornPos) {
	heroId = heroId_;
	pos = bornPos;
	SetDirection(MoveDirections::Down);
	body.SetAnchor(cAnchor);
}

void Hero::SetDirection(MoveDirections d) {
	direction = d;
	frmaeIndexFrom = cFrameIndexRanges[(int)d];
	frmaeIndexTo = cFrameIndexRanges[(int)d + 1];
	frmaeIndex = frmaeIndexFrom;
}

void Hero::ForwardFrame() {
	frmaeIndex += cFrameInc;
	if (frmaeIndex >= frmaeIndexTo) {
		frmaeIndex = frmaeIndexFrom + (frmaeIndex - frmaeIndexTo);
	}
}

void Hero::BackwardFrame() {
	frmaeIndex -= cFrameInc;
	if (frmaeIndex <= frmaeIndexFrom) {
		frmaeIndex = frmaeIndexTo - (frmaeIndexFrom - frmaeIndex);
	}
}

void Hero::Draw() {
	// todo: camera
	body.SetPosition(pos.MakeFlipY()).SetFrame(gLooper.frames_heros[heroId][frmaeIndex]).Draw();
}

xx::Task<> Hero::MainLogic() {
	while (true) {
		if (auto r = gLooper.GetKeyboardMoveInc(); r.has_value()) {
			if (direction != r->first) {
				SetDirection(r->first);
			}
			pos += r->second;
			ForwardFrame();
		}
		co_yield 0;
	}
}
