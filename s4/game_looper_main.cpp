#include "pch.h"

int32_t main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnKeyDown(EmscriptenKeyboardEvent const& e) {
	if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = true;
		return EM_TRUE;
	}
	return EM_FALSE;
}

EM_BOOL GameLooper::OnKeyUp(EmscriptenKeyboardEvent const& e) {
	if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = false;
		return EM_TRUE;
	}
	return EM_FALSE;
}

bool GameLooper::KeyDown(KeyboardKeys k) const {
	return keyboardKeysStates[(KeyboardKeys_t)k];
}

bool GameLooper::KeyDownDelay(KeyboardKeys k, double delaySecs) {
	if (!KeyDown(k)) return false;
	if (keyboardKeysDelays[(KeyboardKeys_t)k] > nowSecs) return false;
	keyboardKeysDelays[(KeyboardKeys_t)k] = nowSecs + delaySecs;
	return true;
}


/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
	touchMode = {};
    mousePos = { (float)e.targetX - windowWidth_2, windowHeight - (float)e.targetY - windowHeight_2 };
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
	touchMode = {};
	mouseBtnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseUp(EmscriptenMouseEvent const& e) {
	mouseBtnStates[e.button] = false;
    return EM_TRUE;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnTouchStart(EmscriptenTouchEvent const& e) {
	touchMode = true;
	if (e.numTouches == 1) {
		auto&& t = e.touches[0];
		aimTouchId = t.identifier;
		aimTouchStartPos = aimTouchMovePos = { (float)t.targetX - windowWidth_2, windowHeight - (float)t.targetY - windowHeight_2 };
	} else {
		for (int32_t i = 0; i < e.numTouches; ++i) {
			auto&& t = e.touches[i];
			if (!t.isChanged) continue;
			fireTouchId = t.identifier;
			mouseBtnStates[0] = true;
			break;
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchMove(EmscriptenTouchEvent const& e) {
	touchMode = true;
	for (int32_t i = 0; i < e.numTouches; ++i) {
		auto&& t = e.touches[i];
		if (!t.isChanged) continue;
		if (aimTouchId == t.identifier) {
			aimTouchMovePos = { (float)t.targetX - windowWidth_2, windowHeight - (float)t.targetY - windowHeight_2 };
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchEnd(EmscriptenTouchEvent const& e) {
	for (int32_t i = 0; i < e.numTouches; ++i) {
		auto&& t = e.touches[i];
		if (!t.isChanged) continue;
		if (aimTouchId == t.identifier) {
			aimTouchId = -1;
			aimTouchStartPos = aimTouchMovePos = {};
		} else if (fireTouchId == t.identifier) {
			fireTouchId = -1;
			mouseBtnStates[0] = false;
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchCancel(EmscriptenTouchEvent const& e) {
	return OnTouchEnd(e);
}
