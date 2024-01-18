#pragma once
#include <game_looper.h>
#include <game_hero.h>
#include <game_logic.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	Env env;

	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
