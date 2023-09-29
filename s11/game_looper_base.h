#pragma once
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

constexpr GDesign<1280, 720, 60> gDesign;

enum class MoveDirections : int {
	Down = 1, Left = 2, Right = 4, Up = 8
};

template<typename Derived>
struct GameLooperBase : Engine<Derived> {
	constexpr static float fps = gDesign.fps, frameDelay = 1.f / fps, maxFrameDelay = 1.f;
	CharTexCache<72> ctc72;
	FpsViewer fv;


	EM_BOOL OnKeyDown(EmscriptenKeyboardEvent const& e) {
		if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
			this->keyboardKeysStates[e.which] = true;
			return EM_TRUE;
		}
		return EM_FALSE;
	}

	EM_BOOL OnKeyUp(EmscriptenKeyboardEvent const& e) {
		if (e.which >= (KeyboardKeys_t)KeyboardKeys::A && e.which <= (KeyboardKeys_t)KeyboardKeys::Z) {
			this->keyboardKeysStates[e.which] = false;
			return EM_TRUE;
		}
		return EM_FALSE;
	}

	bool KeyDown(KeyboardKeys k) const {
		return this->keyboardKeysStates[(KeyboardKeys_t)k];
	}

	bool KeyDownDelay(KeyboardKeys k, double delaySecs) {
		if (!KeyDown(k)) return false;
		if (this->keyboardKeysDelays[(KeyboardKeys_t)k] > this->nowSecs) return false;
		this->keyboardKeysDelays[(KeyboardKeys_t)k] = this->nowSecs + delaySecs;
		return true;
	}


	EM_BOOL OnMouseMove(EmscriptenMouseEvent const& e) {
		touchMode = {};
		this->mouse.pos = { (float)e.targetX - this->windowWidth_2, this->windowHeight - (float)e.targetY - this->windowHeight_2 };
		return EM_TRUE;
	}

	EM_BOOL OnMouseDown(EmscriptenMouseEvent const& e) {
		touchMode = {};
		this->mouse.btnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
		return EM_TRUE;
	}

	EM_BOOL OnMouseUp(EmscriptenMouseEvent const& e) {
		this->mouse.btnStates[e.button] = false;
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
				this->mouse.btnStates[0] = true;
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
				this->mouse.btnStates[0] = false;
			}
		}
		return EM_TRUE;
	}

	EM_BOOL OnTouchCancel(EmscriptenTouchEvent const& e) {
		return OnTouchEnd(e);
	}

	std::optional<std::pair<MoveDirections, XY>> GetKeyboardMoveInc() {
		union Dirty {
			struct {
				uint8_t a, s, d, w;
			};
			uint32_t all{};
		} flags;
		int32_t n = 0;

		if (KeyDown(KeyboardKeys::A)) { flags.a = 1; ++n; }
		if (KeyDown(KeyboardKeys::S)) { flags.s = 1; ++n; }
		if (KeyDown(KeyboardKeys::D)) { flags.d = 1; ++n; }
		if (KeyDown(KeyboardKeys::W)) { flags.w = 1; ++n; }

		if (n > 2) {
			if (flags.a && flags.d) {
				flags.a = flags.d == 0;
				n -= 2;
			}
			if (flags.s && flags.w) {
				flags.s = flags.s == 0;
				n -= 2;
			}
		}
		if (n == 0) return {};

		XY v{};
		MoveDirections md;

		if (n == 2) {
			if (flags.w) {
				md = MoveDirections::Up;
				if (flags.d) {
					(int&)md |= (int)MoveDirections::Right;
					v = { gDesign.sqr2, -gDesign.sqr2 };	// up right
				} else if (flags.a) {
					(int&)md |= (int)MoveDirections::Left;
					v = { -gDesign.sqr2, -gDesign.sqr2 };	// up left
				}
			} else if (flags.s) {
				md = MoveDirections::Down;
				if (flags.d) {
					(int&)md |= (int)MoveDirections::Right;
					v = { gDesign.sqr2, gDesign.sqr2 };		// right down
				} else if (flags.a) {
					(int&)md |= (int)MoveDirections::Left;
					v = { -gDesign.sqr2, gDesign.sqr2 };	// left down
				}
			}
		} else if (n == 1) {
			if (flags.w) {
				md = MoveDirections::Up;
				v.y = -1;	// up
			} else if (flags.s) {
				md = MoveDirections::Down;
				v.y = 1;	// down
			} else if (flags.a) {
				md = MoveDirections::Left;
				v.x = -1;	// left
			} else if (flags.d) {
				md = MoveDirections::Right;
				v.x = 1;	// right
			}
		}

		return std::make_pair(md, v);
	}

};
