#pragma once
#include "looper.h"

struct ItemBase : GridItemBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cRadius{ 16 };

	virtual void DrawFocus() {};

	bool IsFocus() const;

    ItemBase() = default;
    ItemBase(ItemBase const&) = delete;
    ItemBase& operator=(ItemBase const&) = delete;
    ItemBase(ItemBase &&) = default;
    ItemBase& operator=(ItemBase &&) = default;
    virtual ~ItemBase() {};
};

struct MonsterBase : ItemBase {
	static constexpr double cHP{ 100 };
	static constexpr float cSpeed{ 200.f / gDesign.fps };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double hp{};
	float radius{};
	int32_t mapPathIndex{}, trackIndex{};
	float pointIndex{}, speed{}, radians{};
	// todo: more?
};

struct TowerBase : ItemBase {
	int32_t case_focusScale{};							// for focus scale anim
	float focusScale{};									// for focus scale anim

	int32_t case_shootScale{};							// for shoot scale anim
	float shootScale{};									// for shoot scale anim

	void InitFocusAnim();
	void ResetFocusAnim();
	void StepFocusAnim();

	void InitShootAnim();
	void ResetShootAnim();
	void StepShootAnim();

	// todo: more?
};

struct BulletBase : ItemBase {
	static constexpr double cDamage{ 100 };
	static constexpr RGBA8 cColor{ RGBA8_White };

	double damage{};
	float radius{};
	float speed{}, radians{};
	int32_t deathFrameNumber{};
	// todo: more?
};
