#include "pch.h"
#include "main.h"

int main() {
	printf("main()\n");
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
	mouseBtnStates[e.button] = true;
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
		for (int i = 0; i < e.numTouches; ++i) {
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
	for (int i = 0; i < e.numTouches; ++i) {
		auto&& t = e.touches[i];
		if (!t.isChanged) continue;
		if (aimTouchId == t.identifier) {
			aimTouchMovePos = { (float)t.targetX - w / 2, h - (float)t.targetY - h / 2 };
		}
	}
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchEnd(EmscriptenTouchEvent const& e) {
	for (int i = 0; i < e.numTouches; ++i) {
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
	if (e.which >= (int)KeyboardKeys::A && e.which <= (int)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = true;
		return EM_TRUE;
	}
	return EM_FALSE;
}
EM_BOOL GameLooper::OnKeyUp(EmscriptenKeyboardEvent const& e) {
	if (e.which >= (int)KeyboardKeys::A && e.which <= (int)KeyboardKeys::Z) {
		keyboardKeysStates[e.which] = false;
		return EM_TRUE;
	}
	return EM_FALSE;
}

bool GameLooper::Pressed(KeyboardKeys k) const {
	return keyboardKeysStates[(int)k];
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
	printf("Init()\n");
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
	frame_shooter = tp->TryGet("p");			// ready flag

	shooter.Emplace()->Init();	// make player char

	// todo: monsters gen
	while (true) {
		for (size_t i = 0; i < 100; i++) {
			monsters.Emplace().Emplace<Monster1>()->Init();
		}

		co_yield 0;
	}

    co_return;
}

void GameLooper::Update() {
	if (!frame_shooter) return;

	if (shooter) {
		shooter();
	}
	bullets_shooter1.Foreach([&](auto& o) { return !o() || o->disposing; });
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
	effects_explosion.Foreach([&](auto& o) { o->Draw(); });
	effects_damageText.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw

	fv.Draw(ctc72);       // draw fps at corner
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Shooter::Init() {
	Add(MainLogic());
	SetFrame(gLooper.frame_shooter).SetScale(gScale);
}
void Shooter::Draw() {
	Quad::Draw();
}
xx::Task<> Shooter::MainLogic() {
	while (true) {
		float r, sr, cr;
		bool needFire{};

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
		} else {
			if (auto inc = GetKeyboardMoveInc(); inc.has_value()) {
				AddPosition(*inc);
			}
			auto v = gLooper.mousePos - pos;
			r = std::atan2(v.y, v.x);
			sr = std::sin(r);
			cr = std::cos(r);
			needFire = gLooper.mouseBtnStates[0];
		}
		SetRotate(M_PI * 2 - r);

		if (needFire) {
			XY inc{ cr, sr };
			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc * cBulletSpeed, r);
			for (size_t i = 1; i <= 5; ++i) {
				auto r1 = r + 0.1f * (float)i;
				inc = { std::cos(r1), std::sin(r1) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc * cBulletSpeed, r);
				auto r2 = r - 0.1f * (float)i;
				inc = { std::cos(r2), std::sin(r2) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * cFireDistance, inc * cBulletSpeed, r);
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
	int n = 0;

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
	radius = cRadius;
	Add(MainLogic());
	SetFrame(gLooper.frames_monster_3[2]).SetScale(gScale);
	radians = radians_;
	inc = inc_;
	pos = bornPos;
}
void ShooterBullet1::Draw() {
	Quad::Draw();
}
xx::Task<> ShooterBullet1::MainLogic() {
	while (true) {
		AddPosition(inc);
		if ((pos.x > gLooper.w / 2 + cRadius * 2) || (pos.x < -gLooper.w / 2 - cRadius * 2) ||
			(pos.y > gLooper.h / 2 + cRadius * 2) || (pos.y < -gLooper.h / 2 - cRadius * 2)) break;
		// todo: hit check?
		if (gLooper.rnd.Next<bool>()) {
			gLooper.effects_damageText.Emplace().Emplace()->Init(pos, gLooper.rnd.Next<int>(100, 500));
		}
		co_yield 0;
	}
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void DamageText::Init(XY const& bornPos, int hp) {
	Add(MainLogic());
	pos = bornPos;
	txt = std::to_string(hp);
}
void DamageText::Draw() {
	gLooper.ctc24.Draw(pos, txt);
}
xx::Task<> DamageText::MainLogic() {
	XY inc{ 0, 1 };
	int life{ cLife };
	while (--life >= 0) {
		AddPosition(inc);
		co_yield 0;
	}
	co_return;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

void Monster1::Init() {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
	pos.x = gLooper.rnd.Next<float>(-gLooper.w / 2, gLooper.w / 2);
	pos.y = gLooper.rnd.Next<float>(-gLooper.h / 2, gLooper.h / 2);
	scale = {};
}
void Monster1::Draw() {
	SetFrame(gLooper.frames_monster_1[(int)frameIndex]).Draw();
}
xx::Task<> Monster1::MainLogic() {
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

void Monster2::Init() {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
}
void Monster2::Draw() {}
xx::Task<> Monster2::MainLogic() { co_return; }

/*****************************************************************************************************/
/*****************************************************************************************************/

void Monster3::Init() {
	type = cType;
	radius = cRadius;
	Add(MainLogic());
}
void Monster3::Draw() {}
xx::Task<> Monster3::MainLogic() { co_return; }

/*****************************************************************************************************/
/*****************************************************************************************************/

void Explosion::Init() {}
void Explosion::Draw() {}
xx::Task<> Explosion::MainLogic() { co_return; }
