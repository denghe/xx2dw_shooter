#pragma once
#include <game_looper.h>

struct SceneTest1;

struct MonsterBase {
	SceneTest1* scene{};
	XY pos{};
	float scale{}, radius{}, radians{};
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

struct Monster1 : MonsterBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 1 };
	static constexpr float cRadius{ cScale * 7 };

	int tracksIndex{};
	float pointIndex{}, speed{};

	void Init(SceneTest1* scene_, int tracksIndex_, float pointIndex_, float speed_);
	virtual void Update() override;
	virtual void Draw() override;
};

struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	xx::Listi32<xx::Shared<MonsterBase>> monsters;	// todo: phys


	static constexpr int trackCount{ 10 };
	static constexpr int trackDrawStep{ 100 };
	static constexpr float trackStep{ 0.1f };
	static constexpr float trackWidth{ 100 };
	static constexpr float trackMargin{ trackWidth / trackCount };
	static constexpr float trackBaseSpeed{ 5 };
	MovePathCache mpc;
	xx::Listi32<xx::Listi32<XY>> tracks;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
