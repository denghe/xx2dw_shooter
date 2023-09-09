#include "pch.h"

void Shooter::Init(XY const& bornPos) {
	Add(MainLogic());
	pos = bornPos;

	body.SetFrame(gLooper.frame_shooter).SetScale(gScale);
}

void Shooter::Draw() {
	body.SetRotate(radians).Draw();
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
					pos += XY{ cr, sr } * cSpeed * gScale / 4;
				}
			}
			needFire = gLooper.fireTouchId != -1;								// todo: touch special area for special fire ?
		} else {
			if (auto inc = GetKeyboardMoveInc(); inc.has_value()) {
				pos += *inc;
			}
			auto v = gLooper.mousePos/* - pos*/;
			r = std::atan2(v.y, v.x);
			sr = std::sin(r);
			cr = std::cos(r);
			needFire = gLooper.mouseBtnStates[0];
		}

		r = -r;
		auto d = r - radians;
		if (d * d > M_PI * M_PI) {
			if (r < radians) {
				r += M_PI * 2;
			} else {
				r -= M_PI * 2;
			}
		}
		if (r > radians) {
			if (r - radians <= cFrameMaxChangeRadian) {
				radians = r;
			} else {
				radians += cFrameMaxChangeRadian;
			}
			if (radians > M_PI) {
				radians -= M_PI * 2;
			}
		} else {
			if (radians - r <= cFrameMaxChangeRadian) {
				radians = r;
			} else {
				radians -= cFrameMaxChangeRadian;
			}
			if (radians < -M_PI) {
				radians += M_PI * 2;
			}
		}

		r = -radians;
		sr = std::sin(r);
		cr = std::cos(r);

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

	return v * cSpeed * gScale / 4;
}
