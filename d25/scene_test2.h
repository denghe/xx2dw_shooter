#pragma once
#include "looper.h"

// tower defense: map editor

struct SceneTest2;
inline SceneTest2* gSceneEditor;		// init by SceneTest2::Init()

// todo: monsters

enum class MoveTips {
	Left, Right, Up, Down, Begin, End
};

struct MapPath {
	struct Node {
		MoveTips mt;
		int32_t next;
	};
	xx::Listi32<Node> mapMoveTips;
	int32_t beginIdx{}, endIdx{};
};

struct SceneTest2 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;

	xx::Ref<TMX::Map> map;
	xx::Listi32<xx::Ref<Frame>> mapFrames;
	xx::Listi32<MapPath> mapPaths;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
