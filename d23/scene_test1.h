#pragma once
#include <game_looper.h>
#include <game_effect_number.h>

struct SceneTest1;
struct SceneItem : SceneItemBase<SceneTest1> {};

struct Bullet : SceneItem {
	static constexpr int cTypeId{ 1 };

	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 1.f };
	static constexpr float cRadius{ 16.f * cScale };
	static constexpr xx::FromTo cSpeed{ 5.f, 20.f };
	static constexpr float cTimeSpan{ 2 };
	static constexpr xx::FromTo cMaxHitCount{ 1, 3 };

	static constexpr int cHitDelayFrames{ int(gDesign.fps / 10) };
	xx::Listi32<std::pair<xx::Weak<SceneItem>, int>> hitBlackList;

	int maxHitCount{};
	float tailRatio{};
	xx::Task<> mainTask;
	xx::Task<> MainTask();

	void Init(ItemManagerBase* im_);
	void Init(ItemManagerBase* im_, float radians_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};

struct BigMonster : SceneItem {
	static constexpr int cTypeId{ 2 };

	static constexpr XY cAnchor{ 0.5f, 0.5f };
	static constexpr float cScale{ 10.f };
	static constexpr float cRadius{ 16.f * cScale };

	void Init(ItemManagerBase* im_);
	virtual bool Update() override;
	virtual void Draw(Camera const& camera) override;
};

struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	ItemManager<Bullet, BigMonster> im;
	EffectNumberManager1 enm;
	int numBulletGenerateByEveryFrame{1};

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
