#include "pch.h"
#include "main.h"

int main() {
	printf("main()\n");
    emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
        return gLooper.JsLoopCallback(ms);
    }, nullptr);
}
GameLooper gLooper;

EM_BOOL GameLooper::OnMouseMove(EmscriptenMouseEvent const& e) {
    mousePos = { (float)e.targetX - w / 2, h - (float)e.targetY - h / 2 };
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseDown(EmscriptenMouseEvent const& e) {
	mouseBtnStates[e.button] = true;
    return EM_TRUE;
}
EM_BOOL GameLooper::OnMouseUp(EmscriptenMouseEvent const& e) {
	mouseBtnStates[e.button] = false;
    return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchStart(EmscriptenTouchEvent const& e) {
	auto&& t = e.touches[0];
	mousePos = { (float)t.targetX - w / 2, h - (float)t.targetY - h / 2 };
	mouseBtnStates[0] = true;
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchMove(EmscriptenTouchEvent const& e) {
	auto&& t = e.touches[0];
	mousePos = { (float)t.targetX - w / 2, h - (float)t.targetY - h / 2 };
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchEnd(EmscriptenTouchEvent const& e) {
	mouseBtnStates[0] = false;
	return EM_TRUE;
}

EM_BOOL GameLooper::OnTouchCancel(EmscriptenTouchEvent const& e) {
	mouseBtnStates[0] = false;
	return EM_TRUE;
}

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

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
	printf("Init()\n");
}


/*****************************************************************************************************/
/*****************************************************************************************************/

xx::Task<> GameLooper::MainTask() {
    ctc24.Init();

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
		for (size_t i = 0; i < 10; i++) {
			monsters_1.Emplace().Emplace()->Init();
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
	monsters_1.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters_2.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters_3.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_explosion.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_damageText.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (!frame_shooter) return;

	if (shooter) {
		shooter->Draw();
	}
	monsters_1.Foreach([&](auto& o) { o->Draw(); });
	monsters_2.Foreach([&](auto& o) { o->Draw(); });
	monsters_3.Foreach([&](auto& o) { o->Draw(); });
	bullets_shooter1.Foreach([&](auto& o) { o->Draw(); });
	effects_explosion.Foreach([&](auto& o) { o->Draw(); });
	effects_damageText.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw

	fv.Draw(ctc24);       // draw fps at corner
}

void Shooter::Init() {
	Add(MainLogic());
	SetFrame(gLooper.frame_shooter).SetScale(gScale);
}
void Shooter::Draw() {
	Quad::Draw();
}
xx::Task<> Shooter::MainLogic() {
	while (true) {
		if (auto [moved, inc] = GetASDWMoveInc(); moved) {
			AddPosition(inc);
			// todo: limit ?
		}

		auto v = gLooper.mousePos - pos;
		auto r = std::atan2(v.y, v.x);
		SetRotate(M_PI * 2 - r);

		if (gLooper.mouseBtnStates[0]) {
			XY inc{ std::cos(r), std::sin(r) };
			gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * fireDistance, inc * bulletSpeed, r);
			for (size_t i = 1; i <= 5; ++i) {
				auto r1 = r + 0.1f * (float)i;
				inc = { std::cos(r1), std::sin(r1) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * fireDistance, inc * bulletSpeed, r);
				auto r2 = r - 0.1f * (float)i;
				inc = { std::cos(r2), std::sin(r2) };
				gLooper.bullets_shooter1.Emplace().Emplace()->Init(pos + inc * fireDistance, inc * bulletSpeed, r);
			}
		}

		co_yield 0;
	}
}

std::pair<bool, XY> Shooter::GetASDWMoveInc() {
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
	if (n == 0) return { false, XY{} };

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

	return { true, v * speed };
}

void ShooterBullet1::Init(XY const& pos_, XY const& inc_, float radians_) {
	Add(MainLogic());
	SetFrame(gLooper.frames_monster_3[2]).SetScale(gScale);
	radians = radians_;
	inc = inc_;
	pos = pos_;
}
void ShooterBullet1::Draw() {
	Quad::Draw();
}
xx::Task<> ShooterBullet1::MainLogic() {
	while (true) {
		AddPosition(inc);
		if ((pos.x > gLooper.w / 2 + diameter) || (pos.x < -gLooper.w / 2 - diameter) || 
			(pos.y > gLooper.h / 2 + diameter) || (pos.y < -gLooper.h / 2 - diameter)) break;
		// todo: hit check?
		if (gLooper.rnd.Next<bool>()) {
			gLooper.effects_damageText.Emplace().Emplace()->Init(pos, gLooper.rnd.Next<int>(100, 500));
		}
		co_yield 0;
	}
}



void DamageText::Init(XY const& pos_, int hp_) {
	Add(MainLogic());
	pos = pos_;
	hp = hp_;
	txt = std::to_string(hp);
	inc = { 0, 1 };
}
void DamageText::Draw() {
	gLooper.ctc24.Draw(pos, txt);
}
xx::Task<> DamageText::MainLogic() {
	while (--life >= 0) {
		AddPosition(inc);
		co_yield 0;
	}
	co_return;
}


void Monster1::Init() {
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
		frameIndex += frameInc;
		if (frameIndex >= frameMaxIndex) {
			frameIndex -= frameMaxIndex;
		}
		co_yield 0;
	}

	while (scale.x > 0.1f) {
		scale.x -= 0.1f;
		scale.y -= 0.1f;
		co_yield 0;
	}
}

void Monster2::Init() {}
void Monster2::Draw() {}
xx::Task<> Monster2::MainLogic() { co_return; }

void Monster3::Init() {}
void Monster3::Draw() {}
xx::Task<> Monster3::MainLogic() { co_return; }

void Explosion::Init() {}
void Explosion::Draw() {}
xx::Task<> Explosion::MainLogic() { co_return; }
