#include "pch.h"

void ShooterBullet1::Init(XY const& bornPos, XY const& inc_, float radians_) {
	Add(MainLogic());
	SetFrame(gLooper.frames_bullets[cFrameIndex]).SetScale(gScale);
	radians = M_PI * 2 + M_PI / 2 - radians_;
	inc = inc_ * cSpeed;
	pos = bornPos;
	auto c = gLooper.frameNumber;
	if (c % 3 == 0) {
		color = { uint8_t((c + 85) % 256), uint8_t((c + 161) % 256) , uint8_t(c % 256), 255 };
	} else if (c % 3 == 1) {
		color = { uint8_t((c + 161) % 256), uint8_t(c % 256), uint8_t((c + 85) % 256), 255 };
	} else {
		color = { uint8_t((c + 161) % 256), uint8_t((c + 85) % 256), uint8_t(c % 256), 255 };
	}
}
xx::Task<> ShooterBullet1::MainLogic() {
	while (true) {
		AddPosition(inc);
		if ((pos.x > gLooper.w / 2 + cRadius * 2) || (pos.x < -gLooper.w / 2 - cRadius * 2) ||
			(pos.y > gLooper.h / 2 + cRadius * 2) || (pos.y < -gLooper.h / 2 - cRadius * 2)) break;

		co_yield 0;
	}
}
