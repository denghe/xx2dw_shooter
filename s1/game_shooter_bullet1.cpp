#include "pch.h"

void ShooterBullet1::Init(XY const& bornPos, XY const& inc_, float radians_) {
	Add(MainLogic());
	SetFrame(gLooper.frames_bullets[cFrameIndex]).SetScale(gScale);
	radians = M_PI * 2 + M_PI / 2 - radians_;
	inc = inc_ * cSpeed;
	pos = bornPos;
}
xx::Task<> ShooterBullet1::MainLogic() {
	while (true) {
		AddPosition(inc);
		if ((pos.x > gLooper.windowWidth_2 + cRadius * 2) || (pos.x < -gLooper.windowWidth_2 - cRadius * 2) ||
			(pos.y > gLooper.windowHeight_2 + cRadius * 2) || (pos.y < -gLooper.windowHeight_2 - cRadius * 2)) break;

		if (auto r = gLooper.FindNeighborMonster(pos, cRadius)) {
			// todo: - hp ?
			gLooper.effects_damageText.Emplace().Emplace()->Init(pos, gLooper.rnd.Next<int32_t>(1, 500));
			gLooper.effects_explosion.Emplace().Emplace()->Init(pos);
			r->RemoveFromOwner();	// dispose monster
			break;	// suicide
		}

		co_yield 0;
	}
}
