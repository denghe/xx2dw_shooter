#pragma once
#include "looper.h"
#include "effect_explosion.h"
#include <xx_space.h>

namespace Test1 {

	struct Cfg {
		static constexpr float unitSize{ 32.f };
		static constexpr float unitSize_2 { unitSize / 2 };

		static constexpr int32_t numRows{20}, numCols{38};
		static constexpr XY mapSize{ unitSize * numCols, unitSize * numRows };
		static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
		static constexpr XY mapCenterPos { mapSize.x / 2.f, mapSize.y / 2.f };
	};
	inline Cfg gCfg;

	struct Shape {
		XY pos{};
	};

	struct Box : Shape {
		XY size{};
		xx::FromTo<XY> xy;
		void BoxInit(XY const& pos_, XY const& size_);
	};

	struct Block : Box {
		int32_t hp{};
		void Init(XY const& pos_, XY const& size_, int32_t hp_);
		void Draw();
	};

	struct Ball : Shape {
		float radius{};
		XY inc{};
		void Init(XY const& pos_, float radius_);
		bool Update();
		void Draw();
	};

	struct Wall {
		xx::FromTo<Vec2<>> cidx;
		xx::FromTo<XY> xy;
		void Init(xx::FromTo<Vec2<>> const& cidx_);
		void Draw();
	};

	struct Bar : Box {
		float speed{};
		void Init(XY const& pos_, XY const& size_, float speed_);
		bool Update();
		void Draw();
	};

	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;
		int32_t frameNumber{};		// for increase game speed
		int32_t gameSpeedRate{};	// game speed
		Camera camera;
		Rnd rnd;
		Effect::ExplosionManager em;
		xx::Task<> mainTask;
		xx::Task<> MainTask();

		xx::Listi32<Wall> walls;
		xx::SpaceGrid<Block> blocks;
		xx::Listi32<Ball> balls;

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
