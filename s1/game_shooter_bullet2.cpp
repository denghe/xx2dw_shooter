#include "pch.h"

void ShooterBullet2::Init(XY const& bornPos, XY const& inc_, float radians_) {
	Add(MainLogic());
	SetFrame(gLooper.frames_bullets[cFrameIndex]).SetScale(gScale);
	radians = M_PI * 2 + M_PI / 2 - radians_;
	pos = bornPos;
	inc = inc_ * cSpeed;
}
xx::Task<> ShooterBullet2::MainLogic() {
	xx::Weak<MonsterBase> tar;
	while (true) {
		if (!tar) {
			if (auto o = gLooper.FindNearestMonster(pos, cMaxLookupDistance)) {
				tar = xx::WeakFromThis(o);
			}
		}
		if (tar) {
			auto v = tar->pos - pos;
			auto r = std::atan2(v.y, v.x);
			radians = M_PI * 2 + M_PI / 2 - r;
			inc = XY{ std::cos(r), std::sin(r) } * cSpeed;
		}

		AddPosition(inc);
		if ((pos.x > gLooper.windowWidth_2 + cRadius * 2) || (pos.x < -gLooper.windowWidth_2 - cRadius * 2) ||
			(pos.y > gLooper.windowHeight_2 + cRadius * 2) || (pos.y < -gLooper.windowHeight_2 - cRadius * 2)) break;

		if (auto r = gLooper.FindNeighborMonster(pos, cRadius)) {
			// todo: - hp ?
			gLooper.effects_damageText.Emplace().Emplace()->Init(pos, gEngine->rnd.Next<int32_t>(1, 500));
			gLooper.effects_explosion.Emplace().Emplace()->Init(pos);
			r->RemoveFromOwner();	// dispose monster
			break;	// suicide
		}

		co_yield 0;
	}
}
