#include "pch.h"

void ShooterBullet1::Draw() {
	auto& c = gLooper.camera;
	body.SetScale(gScale * c.scale)
		.SetRotate(radians)
		.SetPosition(c.ToGLPos(pos))
		.Draw();
}

void ShooterBullet1::Init(XY const& bornPos, XY const& inc_, float radians_) {
	mainLogic = MainLogic();
	radians = float(M_PI * 2 + M_PI / 2 - radians_);
	inc = inc_ * cSpeed;
	pos = bornPos;
	damage = gEngine->rnd.Next(cDamage, cMaxDamage);

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

		// check hit monsters
		if (auto r = gLooper.FindNeighborMonster(pos, cRadius)) {
			r->Hit(damage);		// r maybe deleted
			co_return;
		}

		// check trees
		auto& sg = gLooper.sgabTrees;
		auto minXY = (pos - cRadius).As<int32_t>(), maxXY = (pos + cRadius).As<int32_t>();
		if (minXY.x < 0) minXY.x = 0;
		if (minXY.y < 0) minXY.y = 0;
		if (maxXY.x >= sg.max.x) maxXY.x = sg.max.x - 1;
		if (maxXY.y >= sg.max.y) maxXY.y = sg.max.y - 1;
		if (minXY.x < maxXY.x && minXY.y < maxXY.y) {
			sg.ForeachAABB(minXY, maxXY);
			auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
			for (auto& tree : sg.results) {
				if (Calc::Intersects::BoxCircle<float>((float)tree->_sgabPos.x, (float)tree->_sgabPos.y, (float)gLooper.tileWidth_2, (float)gLooper.tileHeight_2, pos.x, pos.y, cRadius)) {
					co_return;
				}
			}
		}

		co_yield 0;
	}
}
