#pragma once
#include "game_looper.h"

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	virtual void Init() override;
	virtual void Draw() override;
};
