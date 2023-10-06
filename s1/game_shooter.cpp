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
			if (auto inc = gLooper.GetKeyboardMoveInc(); inc.has_value()) {
				AddPosition(inc->second);
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
