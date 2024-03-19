#pragma once
#include "looper.h"
#include "cfg.h"
#include "item_base.h"
#include "shader_ring.h"
#include "track_manager.h"
#include "effect_number.h"
#include "effect_explosion.h"

// tower defense 2

namespace Enemy {
	struct Monster2 : MonsterBase {
		static constexpr int32_t cTypeId{ 0 };

		double hpBak{};

		void Init(double hp_, int32_t mapPathIndex_);
		bool Update();
		void Draw();
		void PlayDeathEffect();
	};
}

namespace Tower {
	struct Arrow : TowerBase {
		static constexpr int32_t cTypeId{ 1 };

		static constexpr int32_t cFireAfterDelayFrame{ int32_t(1.f / gDesign.frameDelay) };
		static constexpr float cAttackRange{ gCfg.unitSize * 5.5f };
		static constexpr double cDamage{ 10 };

		int32_t level{};
		double damage{};
		int32_t nextFireFrame{};

		void Init(int32_t colIdx, int32_t rowIdx);
		bool Update();
		void Draw();

		virtual void DrawFocus() override;
	};

	struct Cannon : TowerBase {
		static constexpr int32_t cTypeId{ 2 };

		int32_t level{};
		double damage{};
		int32_t nextFireFrame{};

		void Init(int32_t colIdx, int32_t rowIdx);
		bool Update();
		void Draw();
	};
}

namespace Bullet {
	namespace Tower {

		struct Arrow : BulletBase {
			static constexpr int32_t cTypeId{ 3 };

			static constexpr float cSpeed{ 500.f / gDesign.fps };
			static constexpr float cTimeSpan{ 1.f };
			static constexpr float cScale{ 0.2f };
			static constexpr float cTailRatio{ 5.f };

			XY inc{};

			void Init(::Tower::Arrow& owner, MonsterBase& tar);
			bool Update();
			void Draw();
		};

		struct Cannon : BulletBase {
			static constexpr int32_t cTypeId{ 4 };

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

struct ScenePlay : Scene {
	int32_t frameNumber{};		// for increase game speed
	int32_t gameSpeedRate{1};	// game speed
	xx::Task<> mainTask;
	xx::Task<> MainTask();

	xx::Shared<Node> rootNode;

	Camera camera;
	Rnd rnd;
	GridsEx<ItemBase
		, ::Enemy::Monster2
		, ::Tower::Arrow
		, ::Tower::Cannon
		, ::Bullet::Tower::Arrow
		, ::Bullet::Tower::Cannon
	> grids;

	TMX::Map* map{};
	TMX::Layer_Tile* layer{};
	xx::Listi32<TrackManager> tms;
	float mapMaxX{}, mapMaxY{};

	bool dragging{}, lastMBState{};
	XY lastMousePos{}, mouseOffset{};


	Shader_RingInstance ringShader;

	GridsWeak focus;

	Effect::ExplosionManager em;

	EffectNumberManager enm;


	virtual void Init() override;
	virtual void BeforeUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
};
inline ScenePlay* gScenePlay;		// init by ScenePlay::Init()
