#pragma once
#include "looper.h"
#include "cfg.h"
#include "game_base.h"
#include "track_manager.h"
#include "map_path.h"
#include "effect_number.h"

// tower defense 2

namespace Enemy {
	struct Monster2 : MonsterBase {
		static constexpr uint32_t cTypeId{ 0 };

		double hpBak{};

		void Init(double hp_, int32_t mapPathIndex_);
		bool Update();
		void Draw();
	};
}

namespace Tower {
	struct Arrow : TowerBase {
		static constexpr uint32_t cTypeId{ 1 };
		static constexpr int32_t cFireAfterDelayFrame{ int32_t(0.01f / gDesign.frameDelay) };
		static constexpr float cAttackRange{ gCfg.unitSize * 3 };
		static constexpr double cDamage{ 30 };

		int32_t level{};
		double damage{};
		int nextFireFrame{};

		void Init(int32_t colIdx, int32_t rowIdx);
		bool Update();
		void Draw();
	};
	struct Cannon : TowerBase {
		static constexpr uint32_t cTypeId{ 2 };
		int32_t level{};
		double damage{};
		int nextFireFrame{};

		void Init(int32_t colIdx, int32_t rowIdx);
		bool Update();
		void Draw();
	};
}

namespace Bullet {
	namespace Tower {
		struct Arrow : BulletBase {
			static constexpr uint32_t cTypeId{ 3 };

			static constexpr float cSpeed{ 200.f / gDesign.fps };
			static constexpr float cScale{ 0.2f };
			static constexpr float cTailRatio{ 5.f };
			static constexpr float cTimeSpan{ 1.f };

			XY inc{};

			void Init(::Tower::Arrow& owner, MonsterBase& tar);
			bool Update();
			void Draw();
		};
		struct Cannon : BulletBase {
			static constexpr uint32_t cTypeId{ 4 };

			static constexpr float cScale{ 1.f };
			static constexpr float cTailRatio{ 2.f };
			static constexpr int32_t cMaxHitCount{ 5 };

			XY inc{};
			//int32_t maxHitCount{};
			//xx::Listi32<GridsWeakCount> hitBlackList;

			void Init(::Tower::Cannon& owner, MonsterBase& tar);
			bool Update();
			void Draw();
		};
	};
	namespace Enemy {
		// todo
	}
}

struct SceneTest2 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	Rnd rnd;
	Grids<::Enemy::Monster2
		, ::Tower::Arrow
		, ::Tower::Cannon
		, ::Bullet::Tower::Arrow
		, ::Bullet::Tower::Cannon
	> grids;

	xx::Ref<TMX::Map> map;
	xx::Listi32<xx::Ref<Frame>> mapFrames;
	xx::Listi32<MapPath> mapPaths;
	float mapMaxX{}, mapMaxY{};

	EffectNumberManager enm;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
inline SceneTest2* gSceneTest2;		// init by SceneTest2::Init()
