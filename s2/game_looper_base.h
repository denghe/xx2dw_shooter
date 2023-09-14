﻿#pragma once
#include "engine_engine.h"
#include "engine_gdesign.h"
#include "engine_chartexcache.h"
#include "engine_fpsviewer.h"
#include "engine_dynamictexturepacker.h"
#include "engine_curvemovepath.h"
#include "engine_spacegrid_box.h"
#include "engine_spacegrid_circle.h"
#include "engine_spacegrid_ringdiffuse.h"
#include "engine_tiledmap_sede.h"
#include "engine_camera.h"

int32_t main();

constexpr GDesign<1280, 800, 60> gDesign;

// type same as EmscriptenKeyboardEvent.what
using KeyboardKeys_t = decltype(EmscriptenKeyboardEvent::which);
enum class KeyboardKeys : KeyboardKeys_t {
	Unknown = 0,
	A = 65,
	B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
	, MAX_VALUE
};

template<typename Derived>
struct GameLooperBase : Engine<Derived> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;
	CharTexCache<72> ctc72;
	FpsViewer fv;

	std::array<bool, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysStates{};
	std::array<double, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysDelays{};

	EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e) {
		if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
			keyboardKeysStates[e.which] = true;
			return EM_TRUE;
		}
		return EM_FALSE;
	}

	EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e) {
		if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
			keyboardKeysStates[e.which] = false;
			return EM_TRUE;
		}
		return EM_FALSE;
	}

	bool KeyDown(KeyboardKeys k) const {
		return keyboardKeysStates[(KeyboardKeys_t)k];
	}

	bool KeyDownDelay(KeyboardKeys k, double delaySecs) {
		if (!KeyDown(k)) return false;
		if (keyboardKeysDelays[(KeyboardKeys_t)k] > this->nowSecs) return false;
		keyboardKeysDelays[(KeyboardKeys_t)k] = this->nowSecs + delaySecs;
		return true;
	}

	XY mousePos;
	std::array<bool, 16> mouseBtnStates{};

	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e) {
		touchMode = {};
		mousePos = { (float)e.targetX - this->windowWidth_2, this->windowHeight - (float)e.targetY - this->windowHeight_2 };
		return EM_TRUE;
	}

	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e) {
		touchMode = {};
		mouseBtnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
		return EM_TRUE;
	}

	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e) {
		mouseBtnStates[e.button] = false;
		return EM_TRUE;
	}

	long aimTouchId{ -1 }, fireTouchId{ -1 };
	XY aimTouchStartPos, aimTouchMovePos;	// virtual joy
	bool touchMode{};

	EM_BOOL OnTouchStart(EmscriptenTouchEvent const& e) {
		touchMode = true;
		if (e.numTouches == 1) {
			auto&& t = e.touches[0];
			aimTouchId = t.identifier;
			aimTouchStartPos = aimTouchMovePos = { (float)t.targetX - this->windowWidth_2, this->windowHeight - (float)t.targetY - this->windowHeight_2 };
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

	EM_BOOL OnTouchMove(EmscriptenTouchEvent const& e) {
		touchMode = true;
		for (int32_t i = 0; i < e.numTouches; ++i) {
			auto&& t = e.touches[i];
			if (!t.isChanged) continue;
			if (aimTouchId == t.identifier) {
				aimTouchMovePos = { (float)t.targetX - this->windowWidth_2, this->windowHeight - (float)t.targetY - this->windowHeight_2 };
			}
		}
		return EM_TRUE;
	}

	EM_BOOL OnTouchEnd(EmscriptenTouchEvent const& e) {
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

	EM_BOOL OnTouchCancel(EmscriptenTouchEvent const& e) {
		return OnTouchEnd(e);
	}

};
