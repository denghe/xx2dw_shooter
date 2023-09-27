#include "pch.h"

void Shooter::Init() {
	Add(MainLogic());
	SetFrame(gLooper.frame_shooter).SetScale(gScale);
}
xx::Task<> Shooter::MainLogic() {
	while (true) {
		float r, sr, cr;
		bool needFire{}, needFire2{};

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
			needFire = gLooper.fireTouchId != -1;
			// todo: touch special area for special fire ?
		} else {
			if (auto inc = GetKeyboardMoveInc(); inc.has_value()) {
				AddPosition(*inc);
			}
			auto v = gLooper.mouse.pos - pos;
			r = std::atan2(v.y, v.x);
			sr = std::sin(r);
			cr = std::cos(r);
			needFire = gLooper.mouse.btnStates[0];
			needFire2 = gLooper.mouse.btnStates[2];
		}
		SetRotate(M_PI * 2 - r);

		if (needFire) {
			XY inc{ cr, sr };

			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			for (size_t i = 1; i <= 5; ++i) {
				auto r1 = r + 0.1f * (float)i;
				inc = { std::cos(r1), std::sin(r1) };
				gLooper.bullets_shooter2.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
				auto r2 = r - 0.1f * (float)i;
				inc = { std::cos(r2), std::sin(r2) };
				gLooper.bullets_shooter2.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			}
		}

		if (needFire2) {
			XY inc{ cr, sr };

			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			for (size_t i = 1; i <= 50; ++i) {
				auto r1 = r + 0.1f * (float)i;
				inc = { std::cos(r1), std::sin(r1) };
				gLooper.bullets_shooter2.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
				auto r2 = r - 0.1f * (float)i;
				inc = { std::cos(r2), std::sin(r2) };
				gLooper.bullets_shooter2.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc, r);
			}
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
				v = { gSQ, gSQ };	// up right
			} else if (flags.a) {
				v = { -gSQ, gSQ };	// up left
			}
		} else if (flags.s) {
			if (flags.d) {
				v = { gSQ, -gSQ };	// right down
			} else if (flags.a) {
				v = { -gSQ, -gSQ };	// left down
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
