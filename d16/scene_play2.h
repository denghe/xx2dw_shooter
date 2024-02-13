#pragma once
#include <game_looper.h>

struct ScenePlay2 : Scene {
	xx::Shared<Node> rootNode;

	xx::Listi32<xx::Ref<Frame>> frames_creature_slime;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
