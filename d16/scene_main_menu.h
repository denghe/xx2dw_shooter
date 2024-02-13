﻿#pragma once
#include <game_looper.h>
#include <engine_audio.h>

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	xx::Listi<xx::DataShared> datas;
	Audio audio;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
