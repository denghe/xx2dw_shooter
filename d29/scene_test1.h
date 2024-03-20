#pragma once
#include "looper.h"
#include "effect_explosion.h"

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

	struct Wall {
		XY pos{}, size{};
		xx::FromTo<XY> xy;
		void Init(XY const& pos_, XY const& size_);
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

		virtual void Init() override;
		virtual void BeforeUpdate() override;
		virtual void Update() override;
		virtual void Draw() override;
	};
	inline Scene* gScene;		// init by Init()

}
