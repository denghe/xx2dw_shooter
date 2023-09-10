#include "pch.h"

void ShooterBullet1::Draw() {
	if (gLooper.camera.InArea(pos)) {
		body.SetScale(gScale * gLooper.camera.scale)
			.SetRotate(radians)
			.SetPosition(gLooper.camera.ToGLPos(pos))
			.Draw();
	}
}

void ShooterBullet1::Init(XY const& bornPos, XY const& inc_, float radians_) {
	Add(MainLogic());
	radians = M_PI * 2 + M_PI / 2 - radians_;
	inc = inc_ * cSpeed;
	pos = bornPos;

	body.SetFrame(gLooper.frames_bullets[cFrameIndex]);
	auto c = gLooper.frameNumber;
	if (c % 3 == 0) {
		body.color = { uint8_t((c + 85) % 256), uint8_t((c + 161) % 256) , uint8_t(c % 256), 255 };
	} else if (c % 3 == 1) {
		body.color = { uint8_t((c + 161) % 256), uint8_t(c % 256), uint8_t((c + 85) % 256), 255 };
	} else {
		body.color = { uint8_t((c + 161) % 256), uint8_t((c + 85) % 256), uint8_t(c % 256), 255 };
	}
}

xx::Task<> ShooterBullet1::MainLogic() {
	auto life = cLife;
	while (--life > 0) {
		pos += inc;
		//printf("ShooterBullet1 pos %f %f\n", pos.x, pos.y);
		co_yield 0;
	}
}
