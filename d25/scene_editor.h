#pragma once
#include "looper.h"

// tower defense: map editor

struct SceneEditor;
inline SceneEditor* gSceneEditor;		// init by SceneEditor::Init()

// todo


struct SceneEditor : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
