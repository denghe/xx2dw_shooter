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

		if (auto inc = gLooper.GetKeyboardMoveInc(); inc.has_value()) {
			pos += inc->second * cSpeed;
		}
		auto v = gLooper.mouse.pos/* - pos*/;
		r = std::atan2(v.y, v.x);
		needFire = gLooper.mouse.btnStates[0];

		r = -r;
		auto d = r - radians;
		if (d * d > M_PI * M_PI) {
			if (r < radians) {
				r += float(M_PI * 2);
			} else {
				r -= float(M_PI * 2);
			}
		}
		if (r > radians) {
			if (r - radians <= cFrameMaxChangeRadian) {
				radians = r;
			} else {
				radians += cFrameMaxChangeRadian;
			}
			if (radians > M_PI) {
				radians -= float(M_PI * 2);
			}
		} else {
			if (radians - r <= cFrameMaxChangeRadian) {
				radians = r;
			} else {
				radians -= cFrameMaxChangeRadian;
			}
			if (radians < -M_PI) {
				radians += float(M_PI * 2);
			}
		}

		r = -radians + float(M_PI * 4);
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
