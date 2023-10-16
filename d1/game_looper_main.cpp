#include "pch.h"

EM_BOOL GameLooper::OnTouchStart(EmscriptenTouchEvent const& e) {
	touchMode = true;
	if (e.numTouches == 1) {
		auto&& t = e.touches[0];
		aimTouchId = t.identifier;
		aimTouchStartPos = aimTouchMovePos = { (float)t.targetX - windowSize_2.x, windowSize.y - (float)t.targetY - windowSize_2.y };
	} else {
		for (int32_t i = 0; i < e.numTouches; ++i) {
			auto&& t = e.touches[i];
			if (!t.isChanged) continue;
			fireTouchId = t.identifier;
			mouse.btnStates[0] = true;
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
			aimTouchMovePos = { (float)t.targetX - windowSize_2.x, windowSize.y - (float)t.targetY - windowSize_2.y };
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
			mouse.btnStates[0] = false;
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchCancel(EmscriptenTouchEvent const& e) {
	return OnTouchEnd(e);
}
