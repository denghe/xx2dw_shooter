#pragma once
#include "game_looper_base.h"


template<typename Handler>
struct MouseEventListener {
	uint16_t btn{};

	XY downPos{}, lastPos{};
	double downTime{};

	uint8_t lastState{};
	Handler handler{};
	int eventId{};	// 0: no event   1: down  2: move  3: up  4: cancel?

	// todo: helper funcs?

	void Init(uint16_t btn) {
		this->btn = btn;
	}

	// eventId > 0: need Dispatch
	void Update() {
		if (auto&& state = gEngine->mouseBtnStates[btn]; lastState != state) {
			lastState = state;
			if (state) {	// down
				assert(!handler);
				lastPos = downPos = gEngine->mousePos;
				downTime = gEngine->nowSecs;
				eventId = 1;	// need search handler
			} else {	// up
				if (handler) {
					handler->HandleMouseUp(*this);
					handler = {};
					eventId = {};
				}
			}
		} else {
			if (handler && lastPos != gEngine->mousePos) {	// move
				if ((eventId = handler->HandleMouseMove(*this))) {
					handler = {};
					// todo: if eventId == 1 { lastPos = downPos = gEngine->mousePos; downTime = gEngine->nowSecs; }
					if (eventId == 4) {
						eventId = 0;
					}
				} else {
					lastPos = gEngine->mousePos;
				}
			}
		}
	}

	// eventId > 0: need Dispatch next handler
	template<typename H>
	void Dispatch(H&& h) {
		assert(!handler);
		// todo: switch case ?
		if (h->HandleMouseDown(*this)) {
			handler = std::forward<H>(h);
			eventId = {};
		}
	}
};


struct DragCircle {
	using LT = MouseEventListener<DragCircle*>;
	bool HandleMouseDown(LT& L);
	int HandleMouseMove(LT& L);
	void HandleMouseUp(LT& L);

	void Init(XY const& pos, float radius, int32_t segments);
	void Draw();

	XY pos{}, dxy{};
	float radius{}, radiusPow2{};
	LineStrip border;
};

struct Poly {
	std::array<XY, 4> vertices;
	LineStrip border;
	void Init();
	void Draw();
};

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	xx::Task<> MainTask();

	std::vector<DragCircle> cs;
	DragCircle::LT CL;
	DragCircle* draggingC{};

};
extern GameLooper gLooper;
