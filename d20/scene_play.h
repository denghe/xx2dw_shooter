#pragma once
#include <game_looper.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	virtual void Init() override;
	virtual void Draw() override;
};
