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

void GameLooper::Init() {
    w = gDesign.width;
    h = gDesign.height;
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
	frame_shooter = tp->TryGet("p");

	// todo: monsters gen

    co_return;
}

void GameLooper::Update() {
	if (shooter) {
		shooter();
	}
	monsters_1.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters_2.Foreach([&](auto& o) { return !o() || o->disposing; });
	monsters_3.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_explosion.Foreach([&](auto& o) { return !o() || o->disposing; });
	effects_damageText.Foreach([&](auto& o) { return !o() || o->disposing; });

	// todo: more Update
}

void GameLooper::Draw() {
	if (shooter) {
		shooter->Draw();
	}
	monsters_1.Foreach([&](auto& o) { o->Draw(); });
	monsters_2.Foreach([&](auto& o) { o->Draw(); });
	monsters_3.Foreach([&](auto& o) { o->Draw(); });
	effects_explosion.Foreach([&](auto& o) { o->Draw(); });
	effects_damageText.Foreach([&](auto& o) { o->Draw(); });

	// todo: more Draw

	fv.Draw(ctc24);       // draw fps at corner
}


void Monster1::Init() {}
void Monster1::Draw() {}
xx::Task<> Monster1::MainLogic() { co_return; }

void Monster2::Init() {}
void Monster2::Draw() {}
xx::Task<> Monster2::MainLogic() { co_return; }

void Monster3::Init() {}
void Monster3::Draw() {}
xx::Task<> Monster3::MainLogic() { co_return; }

void Explosion::Init() {}
void Explosion::Draw() {}
xx::Task<> Explosion::MainLogic() { co_return; }

void DamageText::Init() {}
void DamageText::Draw() {}
xx::Task<> DamageText::MainLogic() { co_return; }

void Shooter::Init() {}
void Shooter::Draw() {}
xx::Task<> Shooter::MainLogic() { co_return; }

void ShooterBullet1::Init() {}
void ShooterBullet1::Draw() {}
xx::Task<> ShooterBullet1::MainLogic() { co_return; }
