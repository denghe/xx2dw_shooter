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
