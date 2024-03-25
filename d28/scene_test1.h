#pragma once
#include "looper.h"
#include "effect_explosion.h"

namespace Test1 {


	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;
		Camera camera;
		Effect::ExplosionManager em;

		virtual void Init() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
