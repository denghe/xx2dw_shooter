#pragma once
#include <game_looper.h>
#include <hero.h>

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;

	xx::List<xx::Ref<Hero>, int32_t> heroes;

	virtual void Init() override;
	virtual void Draw() override;
};
