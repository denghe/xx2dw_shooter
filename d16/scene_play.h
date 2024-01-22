#pragma once
#include <game_looper.h>
#include <game_hero.h>
#include <game_logic.h>
#include <game_effect_number.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	EffectNumberManager1 enm;

	Env env;

	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
