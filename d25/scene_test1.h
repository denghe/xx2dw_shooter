#pragma once
#include "looper.h"
//#include "track_manager.h"
//#include "item_base.h"
//
//// tower defense: circle world game play ? all of them ar clecles
//// monster = 1 ~ n circle, cannon too
//// circle texture size: 32x32
//// map design: tile set = 32x32, window size = 1280x720, map rows = 22, cols = 40
//
//struct Monster1 : MonsterBase {
//	static constexpr uint32_t cTypeId{ 0 };
//	static constexpr XY cAnchor{ 0.5f, 0.5f };
//
//	double hpBak{};
//
//	void Init(double hp_);
//	bool Update();
//	void Draw();
//};
//
//// todo: Map, Tower
//
//struct SceneTest1 : Scene {
//	xx::Shared<Node> rootNode;
//	Camera camera;
//	Rnd rnd;
//	TrackManager tm;
//	Grid<Monster1> grid;
//	TMX::Layer_Tile* layer{};
//	
//
//	virtual void Init() override;
//	virtual void Update() override;
//	virtual void Draw() override;
//};
//inline SceneTest1* gSceneTest1;		// init by SceneTest1::Init()
