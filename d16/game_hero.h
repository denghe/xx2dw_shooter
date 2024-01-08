#pragma once
#include <game_drawable_sprite.h>

namespace Hero {
	struct Weapon;
	struct Item;

	struct Character : Sprite {
		xx::FromTo<float> animFrameIndex{};
		xx::CurrentMax<int> hp{};

		float moveSpeed{};							// move distance / fixed update FPS
		float magneticStrength{};					// eat coin distance

		XY weaponBasePos{};							// pivot for all weapons

		xx::Listi32<xx::Shared<Weapon>> weapons;	// calc result ( by items )
		xx::Listi32<xx::Shared<Item>> items;		// inventory

		virtual void DrawHP(Camera const& camera) = 0;
	};

	struct Character_Boy : Character {
		constexpr static XY cAnchor{ 0.5f, 0.15f };
		constexpr static float cRadius{ 6.f };
		constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
		constexpr static float cFrameInc{ 12.f / gDesign.fps };
		constexpr static float cSpeed{ 30.f / gDesign.fps };

		void Init(XY const& bornPos);
		virtual bool Update() override;
		virtual void Draw(Camera const& camera) override;
		virtual void DrawHP(Camera const& camera) override;
		xx::Task<> MainLogic_();

		//void Init() {
		//	hp = hp_;
		//	maxHP = maxHp_;
		//	// todo: set q scale by tex size

		//	SetFrame(gLooper.frames_human_4[0]);
		//	SetScale(scale_);
		//	SetAnchor({ 0.5, 0 });
		//}

		//void DrawHP() {
		//	auto p = pos + XY{ 0, texRect.h * scale.y };
		//	auto&& f = gLooper.hpBarCache->Get(hp, maxHP);
		//	q.SetFrame(f).SetPosition(p).SetScale({ GetSizeScaled().x / gLooper.hpBarCache->cBloodSize.x , 1 }).Draw();
		//}

		//bool Update();
	};

}
