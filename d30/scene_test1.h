#pragma once
#include "looper.h"
#include <xx_spaces.h>

namespace Test1 {
	using FR = xx::ForeachResult;

	struct Cfg {
		static constexpr int32_t unitSizei{ 32 };
		static constexpr float unitSizef{ (float)unitSizei };
		static constexpr float _1_unitSizef{ 1.f / unitSizef };
		static constexpr float unitSize_2f{ unitSizef / 2 };
		static constexpr int32_t numGridCols{ 128 };
		static constexpr int32_t numGridRows{ 128 };
		static constexpr XY mapSize{ numGridCols * unitSizei, numGridRows * unitSizei };
		static constexpr XY mapSize_2{ mapSize / 2 };
	};
	inline Cfg gCfg;

	struct Base {
		XY pos;
		float radius;

		QuadInstanceData& BaseDraw(xx::Ref<Frame> const& f);
		virtual void Draw() = 0;
		virtual ~Base() {}
	};

	struct Hero : Base {
		static constexpr int32_t cTypeId{ 0 };
		void Init(XY const& pos_);
		xx::Task<> UpdateLogic = UpdateLogic_();
		xx::Task<> UpdateLogic_();
		int32_t Update();
		virtual void Draw() override;
	};

	struct Pet : Base {
		static constexpr int32_t cTypeId{ 1 };
		xx::SpaceWeak<Hero> owner;
		int32_t index{};							// map to owner.petsPos
		void Init(Hero& owner_, int32_t index_);
		int32_t Update();
		virtual void Draw() override;
	};

	struct Monster : Base {
		static constexpr int32_t cTypeId{ 2 };
		int32_t Update();
		virtual void Draw() override;
	};

	using SGS = xx::SpaceGrids<Base, Hero, Pet, Monster>;

	struct Scene : ::Scene {
		SGS grids;
		xx::Listi32<Base*> bases;	// for draw order by y
		xx::Listi32<XY> petsPos;
		Camera camera;
		xx::Shared<Node> rootNode;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
