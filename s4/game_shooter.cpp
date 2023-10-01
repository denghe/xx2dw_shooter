#include "pch.h"

void Shooter::Draw() {
	body.SetScale(gScale * gLooper.camera.scale).SetRotate(radians).Draw();	// always 0,0  no need set pos
}

void Shooter::Init(XY const& bornPos) {
	mainLogic = MainLogic();
	pos = bornPos;

	body.SetFrame(gLooper.frame_shooter);
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
					pos += XY{ cr, -sr } * cSpeed;
				}
			}
			needFire = gLooper.fireTouchId != -1;								// todo: touch special area for special fire ?
		} else {
			if (auto inc = GetKeyboardMoveInc(); inc.has_value()) {
				pos += *inc;
			}
			auto v = gLooper.mouse.pos/* - pos*/;
			r = std::atan2(v.y, v.x);
			needFire = gLooper.mouse.btnStates[0];
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

		r = -radians + M_PI * 4;
		cr = std::cos(r);
		sr = std::sin(r);

		if (needFire) {
			XY inc{ cr, -sr };
			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			for (size_t i = 1; i <= 3; ++i) {
				auto r1 = r + 0.1f * (float)i;
				inc = { std::cos(r1), -std::sin(r1) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
				auto r2 = r - 0.1f * (float)i;
				inc = { std::cos(r2), -std::sin(r2) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			}

		}

		co_yield 0;
	}
}

std::optional<XY> Shooter::GetKeyboardMoveInc() {
	union Dirty {
		struct {
			union {
				struct {
					uint8_t a, d;
				};
				uint16_t ad;
			};
			union {
				struct {
					uint8_t s, w;
				};
				uint16_t sw;
			};
		};
		uint32_t all{};
	} flags;
	int32_t n = 0;

	if (gLooper.KeyDown(KeyboardKeys::A)) { flags.a = 255; ++n; }
	if (gLooper.KeyDown(KeyboardKeys::S)) { flags.s = 255; ++n; }
	if (gLooper.KeyDown(KeyboardKeys::D)) { flags.d = 255; ++n; }
	if (gLooper.KeyDown(KeyboardKeys::W)) { flags.w = 255; ++n; }

	if (n > 2) {
		if (flags.ad > 255 << 8) {
			flags.ad = 0;
			n -= 2;
		}
		if (flags.sw > 255 << 8) {
			flags.sw = 0;
			n -= 2;
		}
	}
	if (!n) return {};

	XY v{};

	if (n == 2) {
		if (flags.w) {
			if (flags.d) {
				v = { gDesign.sqr2, -gDesign.sqr2 };	// up right
			} else if (flags.a) {
				v = { -gDesign.sqr2, -gDesign.sqr2 };	// up left
			}
		} else if (flags.s) {
			if (flags.d) {
				v = { gDesign.sqr2, gDesign.sqr2 };		// right down
			} else if (flags.a) {
				v = { -gDesign.sqr2, gDesign.sqr2 };	// left down
			}
		}
	} else if (n == 1) {
		if (flags.w) {
			v.y = -1;	// up
		} else if (flags.s) {
			v.y = 1;	// down
		} else if (flags.a) {
			v.x = -1;	// left
		} else if (flags.d) {
			v.x = 1;	// right
		}
	}

	return v * cSpeed;
}
