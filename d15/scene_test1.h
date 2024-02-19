#pragma once
#include <game_looper.h>

struct SceneTest1;
using BaseItem = SceneItem<SceneTest1>;

struct SnakeBody : BaseItem {
	static constexpr int cTypeId{ 1 };

	static constexpr float cScale{ 1 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	static constexpr float cDistance{ 3.f };

	xx::Weak<SnakeBody> head, prev;
	bool isTail{};

	xx::Task<> mainTask;
	xx::Task<> MainTask();

	void Init(ItemManagerBase* im_, XY const& pos_, xx::Weak<SnakeBody> head_, xx::Weak<SnakeBody> prev_, bool isTail_ = false);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};

struct SceneTest1 : Scene {
	Camera camera;
	ItemManager<SnakeBody> im;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
