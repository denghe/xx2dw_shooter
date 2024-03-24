#pragma once
#include "looper.h"
#include <xx_space.h>

namespace Test1 {

	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
