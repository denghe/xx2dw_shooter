#pragma once
#include "looper.h"
#include <xx_spaces.h>

namespace Test1 {
	using FR = xx::ForeachResult;

	struct Cfg {
		static constexpr int unitSizei{ 32 };
		static constexpr float unitSizef{ (float)unitSizei };
		static constexpr float _1_unitSizef{ 1.f / unitSizef };
		static constexpr int32_t numGridCols{ 128 };
		static constexpr int32_t numGridRows{ 128 };
		static constexpr XY mapSize{ numGridCols * unitSizei, numGridRows * unitSizei };
		static constexpr XY mapSize_2{ mapSize / 2 };
	};
	inline Cfg gCfg;

	struct Base {
		XY pos;
		float radius{};

		QuadInstanceData& BaseDraw();
		virtual void Draw() = 0;
		virtual ~Base() {}
	};

	struct Hero : Base {
		static constexpr int32_t cTypeId{ 0 };
		void Init();
		int Update();
		virtual void Draw() override;
	};

	struct Pet : Base {
		static constexpr int32_t cTypeId{ 1 };
		int Update();
		virtual void Draw() override;
	};

	struct Monster : Base {
		static constexpr int32_t cTypeId{ 2 };
		int Update();
		virtual void Draw() override;
	};

	using SGS = xx::SpaceGrids<Base, Hero, Pet, Monster>;

	struct Scene : ::Scene {
		SGS grids;
		xx::Listi32<Base*> bases;	// for draw order by y

		Camera camera;
		xx::Shared<Node> rootNode;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
