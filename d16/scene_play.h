#pragma once
#include <game_looper.h>
#include <game_hero.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	xx::Shared<Hero> hero;

	Camera camera;

	virtual void Init() override;
	virtual void Draw() override;
};
