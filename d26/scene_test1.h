#pragma once
#include "looper.h"
#include <xx_space.h>

// d15 logic but use grid container

struct SceneTest1;
inline static SceneTest1* gSceneTest1;		// fill by SceneTest1::Init()

struct SnakeBody {
	static constexpr int cTypeId{ 0 };

	static constexpr float cScale{ 1 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cSpeed{ 300.f / gDesign.fps };
	static constexpr float cDistance{ 10.f };
	static constexpr float cMinRadians{ g2PI / 32.f };

	union {
		struct {
			float x, y;
		};
		XY pos;
	};
	int32_t lineNumber{};
	bool isTail{};
	xx::SpaceWeak<SnakeBody> head, prev;
	float radians{};
	XY hPos;

	XY GenRndPos(float radius, float safeRadius);

	void Init(XY const& pos_, xx::SpaceWeak<SnakeBody> head_, xx::SpaceWeak<SnakeBody> prev_, bool isTail_ = false);
	bool Update();
	int UpdateCore();
	void Draw();
};

struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;

	Camera camera;
	Rnd rnd;
	xx::SpaceGrid<SnakeBody> grid;

	void CreateSnake(XY const& headPos, int len);

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
