#pragma once
#include <game_looper.h>

struct Hero : Quad {
	static constexpr float cBarHeight{ 6 };

	float hp{}, maxHP{};
	Quad hpBar;
	Scale9Sprite hpBarBG;

	void Init(float scale_, float hp_, float maxHp_) {
		hp = hp_;
		maxHP = maxHp_;

		SetFrame(gLooper.frames_human_4[0]);
		SetScale(scale_);
		SetAnchor({ 0.5, 0 });

		auto p = GetBarPos();
		XY ps{ scale.x * texRect.w, cBarHeight };
		hpBarBG.Init(0, p, { 0.5, 0.5 }
			, gLooper.s9cfg_hp.GetCornerSize() + ps, gLooper.s9cfg_hp);

		hpBar
			.SetFrame(gLooper.frame_dot_1_22)
			.SetColor({255,0,0,255})
			//.SetColor({204,0,0,255})
			.SetAnchor({ 0, 0.5 });

		Update();
	}

	XY GetBarPos() {
		return pos + XY{ 0, texRect.h * scale.y };
	}

	float GetBarWidth() {
		return scale.x * texRect.w * hp / maxHP;
	}

	void Update() {
		auto p = GetBarPos();
		XY ps{ scale.x * texRect.w, cBarHeight };

		hpBarBG.position = pos + XY{ 0, texRect.h * scale.y };
		hpBarBG.FillTrans();

		auto w_2 = GetBarWidth() / 2;
		hpBar
			.SetScale({ w_2, ps.y / 2 })
			.SetPosition(hpBarBG.position - XY{ ps.x / 2, 0 });
	}

	void Draw() {
		Update();
		Quad::Draw();
	}
	void DrawHP() {
		hpBar.Draw();
		hpBarBG.Draw();
	}
};
