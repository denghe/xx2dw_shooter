#include "pch.h"

void Shooter::Init() {
	Add(MainLogic());
	SetFrame(gLooper.frame_shooter).SetScale(gScale);
}
xx::Task<> Shooter::MainLogic() {
	while (true) {
		float r, sr, cr;
		bool needFire{};

		if (gLooper.touchMode) {
			if (gLooper.aimTouchMovePos == gLooper.aimTouchStartPos) {
				r = touchLastRotation;
			} else {
				auto v = gLooper.aimTouchMovePos - gLooper.aimTouchStartPos;
				touchLastRotation = r = std::atan2(v.y, v.x);
				if (v.x * v.x + v.y * v.y > cTouchDistance * cTouchDistance) {
					cr = std::cos(r);
					sr = std::sin(r);
					AddPosition({ cr * cSpeed, sr * cSpeed });
				}
			}
			needFire = gLooper.fireTouchId != -1;								// todo: touch special area for special fire ?
		} else {
			if (auto inc = GetKeyboardMoveInc(); inc.has_value()) {
				AddPosition(*inc);
			}
			auto v = gLooper.mousePos - pos;
			r = std::atan2(v.y, v.x);
			sr = std::sin(r);
			cr = std::cos(r);
			needFire = gLooper.mouseBtnStates[0];
		}

		// a0 = r0, a90 = rPI/2, a180 = rPI, a181 = r-PI-, a240 = r-PI/2
		r = -r;
		// todo
		if (r >= 0 && radians > 0) {
			auto v = radians - r;
			if (v * v > cFrameMaxChangeRadian * cFrameMaxChangeRadian) {
				radians += v < 0 ? cFrameMaxChangeRadian : -cFrameMaxChangeRadian;
				sr = std::sin(-radians);
				cr = std::cos(-radians);
			} else {
				radians = r;
			}
		} else {
			
		}

		if (needFire) {
			XY inc{ cr, sr };
			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
		}

		co_yield 0;
	}
}

std::optional<XY> Shooter::GetKeyboardMoveInc() {
	union Dirty {
		struct {
			uint8_t a, s, d, w;
		};
		uint32_t all{};
	} flags;
	int32_t n = 0;

	if (gLooper.Pressed(KeyboardKeys::A)) { flags.a = 1; ++n; }
	if (gLooper.Pressed(KeyboardKeys::S)) { flags.s = 1; ++n; }
	if (gLooper.Pressed(KeyboardKeys::D)) { flags.d = 1; ++n; }
	if (gLooper.Pressed(KeyboardKeys::W)) { flags.w = 1; ++n; }

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

	if (n == 2) {
		if (flags.w) {
			if (flags.d) {
				v = { gDesign.sqr2, gDesign.sqr2 };	// up right
			} else if (flags.a) {
				v = { -gDesign.sqr2, gDesign.sqr2 };	// up left
			}
		} else if (flags.s) {
			if (flags.d) {
				v = { gDesign.sqr2, -gDesign.sqr2 };	// right down
			} else if (flags.a) {
				v = { -gDesign.sqr2, -gDesign.sqr2 };	// left down
			}
		}
	} else if (n == 1) {
		if (flags.w) {
			v.y = 1;	// up
		} else if (flags.s) {
			v.y = -1;	// down
		} else if (flags.a) {
			v.x = -1;	// left
		} else if (flags.d) {
			v.x = 1;	// right
		}
	}

	return v * cSpeed;
}
