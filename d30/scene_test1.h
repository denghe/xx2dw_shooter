#pragma once
#include "looper.h"
#include <xx_space.h>

namespace Test1 {

	struct Cfg {
		static constexpr int unitSizei{ 32 };
		static constexpr float unitSizef{ (float)unitSizei };
		static constexpr XYi numGridColsRows{ 128, 128 };
		static constexpr XY mapSize{ numGridColsRows * unitSizei };
		static constexpr XY mapSize_2{ mapSize / 2 };
	};
	inline Cfg gCfg;

	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;
		Camera camera;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
