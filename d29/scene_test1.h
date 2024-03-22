#pragma once
#include "looper.h"
#include "effect_explosion.h"
#include <xx_space.h>

namespace Test1 {

	struct Cfg {
		static constexpr float unitSize{ 32.f };
		static constexpr float unitSize_2 { unitSize / 2 };
		static constexpr XY unitXYSize{ unitSize, unitSize };
		static constexpr XY unitXYSize_2 { unitSize_2, unitSize_2 };

		static constexpr int32_t numRows{20}, numCols{38};
		static constexpr XY mapSize{ unitSize * numCols, unitSize * numRows };
		static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
		static constexpr XY mapCenterPos { mapSize.x / 2.f, mapSize.y / 2.f };
	};
	inline Cfg gCfg;

	struct Box {
		float x, y;
		XY size{};
		xx::FromTo<XY> xy;
		void BoxInit(XY const& pos_, XY const& size_);
	};

	struct Block : Box {
		int32_t hp{};
		void Init(XY const& pos_, XY const& size_, int32_t hp_);
		bool Update();
		void Draw();
	};

	struct Ball {
		XY pos{};
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

	struct GlobalEffect {
		float z{};				// draw order
		std::function<void()> draw;
	};
	struct ZDraw {
		float z;
		std::function<void()>* draw;
	};

	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;
		int32_t frameNumber{};		// for increase game speed
		int32_t gameSpeedRate{};	// game speed
		Camera camera;
		Rnd rnd;
		Effect::ExplosionManager explosionManager;

		xx::Listi32<Wall> walls;

		xx::SpaceGrid<Block> blocks;
		xx::Listi32<int32_t> blockIndexs;
		void Shuffle();

		xx::ListLinki32<Ball> balls;

		xx::BlockList<GlobalEffect> globalEffects;
		xx::Listi32<ZDraw> zdraws;	// for sort draw
		void ShowText(XY const& pos, std::string_view const& txt);

		xx::Task<> MainTask = MainTask_();
		xx::Task<> MainTask_();

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
