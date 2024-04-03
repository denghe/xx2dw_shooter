#pragma once
#include <engine_includes.h>

struct Scene {
	xx::Tasks tasks;
	virtual void Init() {};			// enter: gLooper.scene == old
	virtual ~Scene() {}				//  exit: gLooper.scene == new
	virtual void BeforeUpdate() {};
	virtual void Update() {};
	virtual void Draw() {};
};

#ifdef ENABLE_ENGINE_IMGUI

struct ImGuiScene : Scene {
	virtual void ImGuiUpdate() = 0;
	virtual void Init() override {
		EngineBase1::Instance().imguiUpdate = [this] { this->ImGuiUpdate(); };
	}
	virtual ~ImGuiScene() override {
		EngineBase1::Instance().imguiUpdate = {};
	}
};

#endif
