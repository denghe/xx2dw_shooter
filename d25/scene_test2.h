#pragma once
#include "looper.h"
#include "game_monster_base.h"
#include "track_manager.h"
#include "map_path.h"

// tower defense 2

struct Monster2 : MonsterBase {
	static constexpr uint32_t cTypeId{ 0 };
	static constexpr XY cAnchor{ 0.5f, 0.5f };

	int32_t mapPathIndex{};
	double hpBak{};

	void Init(double hp_, int32_t mapPathIndex_);
	bool Update();
	void Draw();
};

struct SceneTest2 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	Rnd rnd;
	Grid<Monster2> grid;

	xx::Ref<TMX::Map> map;
	xx::Listi32<xx::Ref<Frame>> mapFrames;
	xx::Listi32<MapPath> mapPaths;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
inline SceneTest2* gSceneTest2;		// init by SceneTest2::Init()
