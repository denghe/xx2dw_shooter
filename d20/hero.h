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

		//q.SetFrame(gLooper.frame_dot_1_22).SetAnchor({});
	}

	void DrawHP() {
		auto p = pos + XY{ 0, texRect.h * scale.y };
		auto&& f = gLooper.hpBarCache->Get(hp, maxHP);
		q.SetFrame(f).SetPosition(p).SetScale({ GetSizeScaled().x / gLooper.hpBarCache->cBloodSize.x , 1 }).Draw();

		//auto wh = GetSizeScaled();
		//XY p{ pos.x - wh.x / 2, pos.y + wh.y };
		//XY s{ wh.x / 2, 10 / 2 };
		//q.SetPosition(p).SetColor(RGBA8_Black).SetScale(s).Draw();
		//p += XY{1,1};
		//s = { hp * (wh.x - 2) / maxHP / 2, (10-2) / 2};
		//q.SetPosition(p).SetColor(RGBA8_Red).SetScale(s).Draw();
	}
};
