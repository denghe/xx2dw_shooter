#include "pch.h"
#include "main.h"

int32_t main() {
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
	touchMode = {};
    mousePos = { (float)e.targetX - w / 2, h - (float)e.targetY - h / 2 };
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
	touchMode = {};
	mouseBtnStates[e.button] = true;	// mouse left btn == 0, right btn == 2
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseUp(EmscriptenMouseEvent const& e) {
	mouseBtnStates[e.button] = false;
    return EM_TRUE;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnTouchStart(EmscriptenTouchEvent const& e) {
	touchMode = true;
	if (e.numTouches == 1) {
		auto&& t = e.touches[0];
		aimTouchId = t.identifier;
		aimTouchStartPos = aimTouchMovePos = { (float)t.targetX - w / 2, h - (float)t.targetY - h / 2 };
	} else {
		for (int32_t i = 0; i < e.numTouches; ++i) {
			auto&& t = e.touches[i];
			if (!t.isChanged) continue;
			fireTouchId = t.identifier;
			mouseBtnStates[0] = true;
			break;
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchMove(EmscriptenTouchEvent const& e) {
	touchMode = true;
	for (int32_t i = 0; i < e.numTouches; ++i) {
		auto&& t = e.touches[i];
		if (!t.isChanged) continue;
		if (aimTouchId == t.identifier) {
			aimTouchMovePos = { (float)t.targetX - w / 2, h - (float)t.targetY - h / 2 };
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchEnd(EmscriptenTouchEvent const& e) {
	for (int32_t i = 0; i < e.numTouches; ++i) {
		auto&& t = e.touches[i];
		if (!t.isChanged) continue;
		if (aimTouchId == t.identifier) {
			aimTouchId = -1;
			aimTouchStartPos = aimTouchMovePos = {};
		} else if (fireTouchId == t.identifier) {
			fireTouchId = -1;
			mouseBtnStates[0] = false;
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchCancel(EmscriptenTouchEvent const& e) {
	return OnTouchEnd(e);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

EM_BOOL GameLooper::OnKeyDown(EmscriptenKeyboardEvent const& e) {
	if (e.which >= (int32_t)KeyboardKeys::A && e.which <= (int32_t)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = true;
		return EM_TRUE;
	}
	return EM_FALSE;
}
EM_BOOL GameLooper::OnKeyUp(EmscriptenKeyboardEvent const& e) {
	if (e.which >= (int32_t)KeyboardKeys::A && e.which <= (int32_t)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = false;
		return EM_TRUE;
	}
	return EM_FALSE;
}

bool GameLooper::Pressed(KeyboardKeys k) const {
	return keyboardKeysStates[(int32_t)k];
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
}

xx::Task<> GameLooper::MainTask() {
    ctc24.Init();
    ctc72.Init();

	// preload texs
    auto tp = co_await AsyncLoadTexturePackerFromUrl("res/pics.blist");
	xx_assert(tp);

	tp->GetToByPrefix(frames_monster_1, "ma");
	tp->GetToByPrefix(frames_monster_2, "mb");
	tp->GetToByPrefix(frames_monster_3, "mc");
	tp->GetToByPrefix(frames_explosion, "e");
	tp->GetToByPrefix(frames_bullets, "b");
	frame_shooter = tp->TryGet("p");			// ready flag

	sgc.Init(gGridNumRows, gGridNumCols, gGridCellDiameter);	// init physics grid

	shooter.Emplace()->Init();	// make player char

	// generate monsters

	//// cake
	//printf("sgrdd.idxs.len = %d\n", (int)sgrdd.idxs.len);
	//int i = 0;
	//for (auto& [n, r] : sgrdd.lens) {
	//	//printf("n = %d\n", n);
	//	for (; i < n; ++i) {
	//		//printf("idx = %d %d\n", idxs[i].x, idxs[i].y);
	//		NewMonster<Monster1>(sgrdd.idxs[i].As<float>() * 2);
	//	}
	//	co_yield 0;
	//}

	//// doughnut
	//tasks.Add([this]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 7; i++) {
	//			auto a = rnd.Next<float>(M_PI * 2);
	//			auto r = rnd.Next<float>(256, 384);
	//			NewMonster<Monster1>(XY{ std::cos(a), std::sin(a) } * r);
	//		}
	//		co_yield 0;
	//	}
	//});
	//tasks.Add([this]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 5; i++) {
	//			auto a = rnd.Next<float>(M_PI * 2);
	//			auto r = rnd.Next<float>(128, 256);
	//			NewMonster<Monster2>(XY{ std::cos(a), std::sin(a) } *r);
	//		}
	//		co_yield 0;
	//	}
	//});
	//tasks.Add([this]()->xx::Task<> {
	//	while (true) {
	//		for (size_t i = 0; i < 3; i++) {
	//			auto a = rnd.Next<float>(M_PI * 2);
	//			auto r = rnd.Next<float>(64, 128);
	//			NewMonster<Monster3>(XY{ std::cos(a), std::sin(a) } *r);
	//		}
	//		co_yield 0;
	//	}
	//});
	
	while (true) {
		for (size_t i = 0; i < 30; i++) {
			auto a = rnd.Next<float>(M_PI * 2);
			auto r = rnd.Next<float>(384, 400);
			NewMonster<Monster1>(XY{ std::cos(a), std::sin(a) } *r);
		}
		co_yield 0;
	}
}

void GameLooper::Update() {
	if (!frame_shooter) return;

	if (shooter) {
		shooter();
	}
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });
	bullets_shooter2.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_explosion.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_damageText.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (!frame_shooter) return;

	monsters.Foreach([&](auto& o) {
		switch (o->type) {
		case ObjTypes::Monster1: ((xx::Shared<Monster1>&)o)->Draw(); break;
		case ObjTypes::Monster2: ((xx::Shared<Monster2>&)o)->Draw(); break;
		case ObjTypes::Monster3: ((xx::Shared<Monster3>&)o)->Draw(); break;
		default: xx_assert(false);
		}
	});
	if (shooter) {
		shooter->Draw();
	}
	bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });
	bullets_shooter2.Foreach([&](auto& o) { o->Draw(); });
	effects_explosion.Foreach([&](auto& o) { o->Draw(); });
	effects_damageText.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw

	fv.Draw(ctc72);       // draw fps at corner
}


/*****************************************************************************************************/
/*****************************************************************************************************/

void GridObjBase::RemoveFromOwner() {
	owner->Remove(ivAtOwner);
}

void GridObjBase::GridInit() {
	SGCInit(&gLooper.sgc, gGridBasePos.MakeAdd(pos));
}

void GridObjBase::GridUpdate() {
	SGCUpdate(gGridBasePos.MakeAdd(pos));
}

GridObjBase::~GridObjBase() {
	SGCTryRemove();
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Shooter::Init() {
	type = cType;
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
			auto v = gLooper.mousePos - pos;
			r = std::atan2(v.y, v.x);
			sr = std::sin(r);
			cr = std::cos(r);
			needFire = gLooper.mouseBtnStates[0];
			needFire2 = gLooper.mouseBtnStates[2];
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

/*****************************************************************************************************/
/*****************************************************************************************************/

void ShooterBullet1::Init(XY const& bornPos, XY const& inc_, float radians_) {
	type = cType;
	Add(MainLogic());
	SetFrame(gLooper.frames_bullets[cFrameIndex]).SetScale(gScale);
	radians = M_PI * 2 + M_PI / 2 - radians_;
	inc = inc_ * cSpeed;
	pos = bornPos;
}
xx::Task<> ShooterBullet1::MainLogic() {
	while (true) {
		AddPosition(inc);
		if ((pos.x > gLooper.w / 2 + cRadius * 2) || (pos.x < -gLooper.w / 2 - cRadius * 2) ||
			(pos.y > gLooper.h / 2 + cRadius * 2) || (pos.y < -gLooper.h / 2 - cRadius * 2)) break;

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

/*****************************************************************************************************/
/*****************************************************************************************************/

void ShooterBullet2::Init(XY const& bornPos, XY const& inc_, float radians_) {
	type = cType;
	Add(MainLogic());
	SetFrame(gLooper.frames_bullets[cFrameIndex]).SetScale(gScale);
	radians = M_PI * 2 + M_PI / 2 - radians_;
	pos = bornPos;
	inc = inc_ * cSpeed;
}
xx::Task<> ShooterBullet2::MainLogic() {
	xx::Weak<GridObjBase> tar;
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
		if ((pos.x > gLooper.w / 2 + cRadius * 2) || (pos.x < -gLooper.w / 2 - cRadius * 2) ||
			(pos.y > gLooper.h / 2 + cRadius * 2) || (pos.y < -gLooper.h / 2 - cRadius * 2)) break;

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

/*****************************************************************************************************/
/*****************************************************************************************************/

void DamageText::Init(XY const& bornPos, int32_t hp) {
	Add(MainLogic());
	SetColor({255,0,0,255});
	pos = bornPos;
	txt = std::to_string(hp);
}
void DamageText::Draw() {
	gLooper.ctc24.Draw(pos, txt, color);
}
xx::Task<> DamageText::MainLogic() {
	XY inc{ 0, 1 };
	int32_t life{ cLife };
	do {
		co_yield 0;
		AddPosition(inc);
	} while (--life >= 0);
	constexpr int step = 255 / cLife;
	for (int i = 255; i >= 0; i -= step) {
		SetColorA((uint32_t)i);
		co_yield 0;
	}
	SetColorA(0);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Explosion::Init(XY const& bornPos) {
	Add(MainLogic());
	pos = bornPos;
}
void Explosion::Draw() {
	SetFrame(gLooper.frames_explosion[(int32_t)frameIndex]).Draw();
}
xx::Task<> Explosion::MainLogic() {
	do {
		co_yield 0;
		frameIndex += cFrameInc;
	} while (frameIndex < cFrameMaxIndex);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Monster1::Init(XY const& bornPos) {
	type = cType;
	radius = cRadius * cScale;
	Add(MainLogic());
	scale = { cScale / gSQ, cScale / gSQ };
	pos = bornPos;
	GridInit();
}
void Monster1::Draw() {
	SetFrame(gLooper.frames_monster_1[(int32_t)frameIndex]).Draw();
}
xx::Task<> Monster1::MainLogic() {
	// scale in
	while (scale.x < cScale) {
		scale.x += cScale / 10;
		scale.y += cScale / 10;
		co_yield 0;
	}
	scale = { cScale, cScale };

	while (--life > 0) {

		// step frame anim
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}

		// physics simulate
		// calc neighbor cross force
		XY combineForce{};										// for move vector
		int numCross{}, limit = 8;
		auto p = gGridBasePos.MakeAdd(pos);						// convert pos to grid coordinate
		auto crIdx = _sgc->PosToCrIdx(p);						// calc grid col row index
		_sgc->Foreach9(crIdx, [&](GridObjBase* m) {
			if (m == this) return false;						// skip self
			auto d = pos - m->pos;
			auto rr = (m->radius + radius) * (m->radius + radius);
			auto dd = d.x * d.x + d.y * d.y;
			if (rr > dd) {										// cross?
				++numCross;
				if (dd) {
					combineForce += d / std::sqrt(dd);			// normalize
				}
			}
			return --limit < 0;									// number limit
		});
		//printf("numCross = %d  combineForce %f %f\n", numCross, combineForce.x, combineForce.y);

		// calc ship follow force
		auto d = gLooper.shooter->pos - pos;
		auto dd = d.x * d.x + d.y * d.y;

		// calc new pos
		XY newPos = pos;
		if (numCross) {											// cross?
			if (dd) {
				combineForce += d / std::sqrt(dd) / 100;		// weak force assign for ship follow
				//printf("combineForce = %f %f\n", combineForce.x, combineForce.y);
			}
			//if (combineForce.IsZero()) {						// move by random angle
			if (combineForce.x * combineForce.x < 0.0001 && combineForce.y * combineForce.y < 0.0001) {
				auto r = gLooper.rnd.Next<float>(M_PI * 2);
				newPos += XY{ std::cos(r), std::sin(r) } * cSpeed * 3;
				//printf("r = %f   pos = %f %f   newPos = %f %f\n", r, pos.x, pos.y, newPos.x, newPos.y);
			} else {
				newPos += combineForce.MakeNormalize() * cSpeed;
				//printf("pos = %f %f   newPos = %f %f\n", pos.x, pos.y, newPos.x, newPos.y);
			}
			//newPos += XY{ -0.5f, 0.34f };	// fix group effect	  todo: change calc step to update
		} else {
			if (dd > cSpeed * cSpeed) {							// follow shooter directly
				newPos += d / std::sqrt(dd) * cSpeed;			// normalize
			} else {
				newPos = gLooper.shooter->pos;
			}
		}

		// upgrade space grid
		if (newPos != pos) {
			//printf("pos = %f %f    newPos = %f %f\n", pos.x, pos.y, newPos.x, newPos.y);
			pos = newPos;
			GridUpdate();
		}

		co_yield 0;
	}

	// scale out
	while (scale.x > cScale / 10) {
		scale.x -= cScale / 10;
		scale.y -= cScale / 10;
		co_yield 0;
	}
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Monster2::Init(XY const& bornPos) {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
	scale = {};
	pos = bornPos;
	GridInit();
}
void Monster2::Draw() {
	SetFrame(gLooper.frames_monster_2[(int32_t)frameIndex]).Draw();
}
xx::Task<> Monster2::MainLogic() {
	//GridUpdate();
	while (scale.x < 1.f) {
		scale.x += 0.1f;
		scale.y += 0.1f;
		co_yield 0;
	}
	scale = { 1, 1 };

	while (--life > 0) {
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}
		co_yield 0;
	}

	while (scale.x > 0.1f) {
		scale.x -= 0.1f;
		scale.y -= 0.1f;
		co_yield 0;
	}
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Monster3::Init(XY const& bornPos) {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
	scale = {};
	pos = bornPos;
	GridInit();
}
void Monster3::Draw() {
	SetFrame(gLooper.frames_monster_3[(int32_t)frameIndex]).Draw();
}
xx::Task<> Monster3::MainLogic() {
	//GridUpdate();
	while (scale.x < 1.f) {
		scale.x += 0.1f;
		scale.y += 0.1f;
		co_yield 0;
	}
	scale = { 1, 1 };

	while (--life > 0) {
		frameIndex += cFrameInc;
		if (frameIndex >= cFrameMaxIndex) {
			frameIndex -= cFrameMaxIndex;
		}
		co_yield 0;
	}

	while (scale.x > 0.1f) {
		scale.x -= 0.1f;
		scale.y -= 0.1f;
		co_yield 0;
	}
}
