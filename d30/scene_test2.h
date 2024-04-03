#pragma once
#include "looper.h"

namespace Test2 {
	using FR = xx::ForeachResult;

	struct Cfg {
		static constexpr int32_t numMaxPets{ 1000 };
		static constexpr int32_t unitSizei{ 32 };
		static constexpr float unitSizef{ (float)unitSizei };
		static constexpr float _1_unitSizef{ 1.f / unitSizef };
		static constexpr float unitSize_2f{ unitSizef / 2 };
		static constexpr int32_t numGridCols{ 512 };
		static constexpr int32_t numGridRows{ 512 };
		static constexpr XY mapSize{ numGridCols * unitSizei, numGridRows * unitSizei };
		static constexpr XY mapSize_2{ mapSize / 2 };
		static constexpr int32_t petIndexRotateStep{ 1000 };
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
		float petPosIndex{};
		void Init(XY const& pos_);
		xx::Task<> UpdateLogic = UpdateLogic_();
		xx::Task<> UpdateLogic_();
		int32_t Update();
		virtual void Draw() override;
	};

	struct Pet : Base {
		static constexpr int32_t cTypeId{ 1 };
		xx::SpacesWeak<Hero> ownerWeak;
		int32_t petIndex{};
		void Init(Hero& owner_, int32_t petIndex_);
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
		xx::Listi32<xx::Listi32<XY>> petsPoss;
		Camera camera;
		xx::Shared<Node> rootNode;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
