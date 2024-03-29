#pragma once
#include "looper.h"

struct SceneMainMenu : Scene {
	xx::Shared<Node> rootNode;

	bool showDemoWindow{ true };
	void ImGuiUpdate();

	virtual void Init() override;
	virtual void Draw() override;
	virtual ~SceneMainMenu() override;
};
