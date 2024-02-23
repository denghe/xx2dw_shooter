#pragma once
#include <game_looper.h>

// todo: Swinging the tail performance compare

struct TailBase {
	XY pos{};
	float radians{}, radius{};
};

struct Tail;
struct TailCell : TailBase {
	static constexpr float cRadius{ 4 };
	float distance{};
	float radiansRange{};
	xx::Weak<Tail> tail;
	xx::Weak<TailBase> prev;

	void Calc();
	void Init(xx::Weak<Tail> tail_, xx::Weak<TailBase> prev_, float distance_, float radiansRange_);
	void Update();
	void Draw(Camera const& camera);
};

struct Tail : TailBase {
	static constexpr float cCurvatureSpeed{ 1 };
	static constexpr float cCurvatureStep{ 1 / gDesign.fps * cCurvatureSpeed };

	xx::Weak<TailBase> owner;
	float ownerOffsetRadians{};

	float curvature{};	// ~1 ~ 1
	float scale{};

	xx::Listi32<xx::Shared<TailCell>> cells;

	xx::Task<> mainTask;
	xx::Task<> MainTask();

	template<typename F>
	void ForeachCell(F&& func) {
		for (auto& c : cells) func(c);
	}

	void Calc();
	Tail& Init(XY const& pos_, float radians_, int len_, float scale_ = 1);
	Tail& Init(xx::Weak<TailBase> owner_, float ownerOffsetRadians_, int len_, float scale_ = 1);
	void InitCore(int len_);
	void Update();
	void Draw(Camera const& camera);
};

struct SceneTest1 : Scene {
	Camera camera;
	xx::Listi32<xx::Shared<Tail>> tails;
	xx::Shared<Tail> MakeTail(XY const& pos_, float radians_, int len_, float scale_, xx::Shared<Tail> parent = {});
	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
