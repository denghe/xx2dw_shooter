#pragma once
#include <game_looper.h>
#include <engine_scrollview.h>
// todo: include from engine_base3

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	virtual void Init() override;
	virtual void Draw() override;
};
