#pragma once
#include <game_looper.h>

struct Hero : Quad {
	int hp{}, maxHP{};	// todo: int
	Quad q;

	void Init(float scale_, int hp_, int maxHp_) {
		hp = hp_;
		maxHP = maxHp_;
		// todo: set q scale by tex size

		SetFrame(gLooper.frames_human_4[0]);
		SetScale(scale_);
		SetAnchor({ 0.5, 0 });
	}

	void DrawHP() {
		auto p = pos + XY{ 0, texRect.h * scale.y };
		auto&& f = gLooper.hpBarCache->Get(hp, maxHP);
		q.SetFrame(f).SetPosition(p).SetScale({ GetSizeScaled().x / gLooper.hpBarCache->cWidth , 1 }).Draw();
	}
};
