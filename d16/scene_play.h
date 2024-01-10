#pragma once
#include <game_looper.h>
#include <game_hero.h>
#include <game_item.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	ItemManager im;

	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
