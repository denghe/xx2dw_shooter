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

		static constexpr int32_t numRows{24}, numCols{39};
		static constexpr XY mapSize{ unitSize * numCols, unitSize * numRows };
		static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };
		static constexpr XY mapCenterPos { mapSize.x / 2.f, mapSize.y / 2.f };

		static constexpr int32_t updateMultipleTimes{ 10 };
		static constexpr float ballSpeed{ 500.f / gDesign.fps / (float)updateMultipleTimes };
		static constexpr xx::FromTo<float> barSpaceX{ unitSize, mapSize.x - unitSize };
		static constexpr float barSpeed{ 3000.f / gDesign.fps };
		static constexpr xx::FromTo<float> barShootAngle{ gNPI + 0.9f, -0.9f };	// 0.1

		static constexpr int32_t defaultGameSpeedRate{ 1 };
	};
	inline Cfg gCfg;

	struct Box {
		union {
		struct {
		float x, y;
		};
		XY pos;
		};
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
		union {
			struct {
				float x, y;
			};
			XY pos;
		};
		float radius{}, radians{}, speed{};
		RGBA8 color;
		xx::Task<> MainTask = MainTask_();
		xx::Task<> MainTask_();
		void Init(XY const& pos_, float radius_, float radians_, float speed_, RGBA8 color_ = { 255,255,255,127 });
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
		std::reference_wrapper<std::function<void()>> draw;
	};

	// todo: sound

	struct Scene : ::Scene {
		xx::Shared<Node> rootNode;
		int32_t gameSpeedRate{};
		int32_t frameNumber{};
		Camera camera;
		Rnd rnd;

		xx::Listi32<Wall> walls;

		xx::SpaceGrid<Block> blocks;
		xx::Listi32<int32_t> blockIndexs;

		xx::BlockLink<Ball> balls;

		xx::Shared<Bar> bar;

		Effect::ExplosionManager explosionManager;

		xx::BlockLink<GlobalEffect> globalEffects;
		xx::Listi32<ZDraw> zdraws;	// for sort


		// event handlers
		std::function<void(Bar&)> onBarBorn{ [](auto&) {} };
		std::function<void(Ball&)> onBallBorn{ [](auto&) {} };
		std::function<void(Ball&)> onBallDead{ [](auto&) {} };
		std::function<void(Ball&, Block&)> onBallHitBlock{ [](auto&, auto&) {} };
		std::function<void(Ball&, Block&)> onBallKillBlock{ [](auto&, auto&) {} };
		// ...
		xx::CondTasks<std::function<bool()>> eventTasks;
		

		void ShuffleBlockIndexs();
		void ShowText(XY const& pos, std::string_view const& txt);

		xx::Task<> AsyncSleep(float secs);
		xx::Task<> MainTask = MainTask_();
		xx::Task<> MainTask_();

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
