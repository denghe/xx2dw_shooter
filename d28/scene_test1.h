#pragma once
#include "looper.h"

struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
inline SceneTest1* gSceneTest1;		// init by Init()
