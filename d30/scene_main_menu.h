#pragma once
#include "looper.h"

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	virtual void Init() override;
	virtual void Draw() override;
};
