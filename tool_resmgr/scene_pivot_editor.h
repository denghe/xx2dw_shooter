#pragma once
#include "pch.h"

namespace Scene_PivotEditor {

	struct Scene : ::Scene {
		Camera camera;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
