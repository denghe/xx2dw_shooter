#include "pch.h"
#include "scene_play.h"
#include "scene_main_menu.h"

namespace Bullet::Tower {

	void Cannon::Init(::Tower::Cannon& owner, MonsterBase& tar) {
		//draw = [](void* self) { ((Cannon*)self)->Draw(); };
		// todo
		//maxHitCount = cMaxHitCount;
		//hitBlackList.Reserve(maxHitCount);
	}

	bool Cannon::Update() {

		//// hit check	( lambda return false mean foreach next m )
		//gSceneTest2->grids.Get<::Enemy::Monster2>().Foreach9([&](::Enemy::Monster2& m)->bool {
		//	// intersects ?
		//	if (!Calc::Intersects::CircleCircle<float>(
		//		pos.x, pos.y, radius, m.pos.x, m.pos.y, m.radius)) return false;

		//	// black list check
		//	for (int i = hitBlackList.len - 1; i >= 0; --i) {
		//		auto& o = hitBlackList[i];

		//		if (!o.Is<BigMonster>()) {
		//			if (/*!o.Exists2() || */o.value <= scene->frameNumber) {
		//				hitBlackList.SwapRemoveAt(i);
		//			}
		//			continue;
		//		}

		//		if (!o.Exists<BigMonster>()) {
		//			hitBlackList.SwapRemoveAt(i);
		//			continue;
		//		}

		//		if (o.iv == m.iv) {
		//			// not timeout ?
		//			if (o.value > scene->frameNumber) return false;

		//			// renew timestamp
		//			o.value = scene->frameNumber + cHitDelayFrames;
		//			goto LabHit;
		//		}
		//	}

		//	// add to black list 
		//	hitBlackList.Emplace(PointerInt{ {m.typeId, m.iv}, scene->frameNumber + cHitDelayFrames });

		//LabHit:
		//	// hit effect
		//	scene->enm.Add(pos, XY{ 0, -1 }, RGBA8_Red, int(radius * 1000));
		//	--maxHitCount;
		//	return maxHitCount == 0;
		//});

		//if (maxHitCount == 0) COR_EXIT;		// todo: fade out ?
		//pos += inc;

		return false;
	}
	void Cannon::Draw() {}

}
