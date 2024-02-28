#pragma once
#include <game_looper.h>

struct EyeBase {
	static constexpr XY cAnchor{ 0.5f, 0.5f };
	XY pos{};
	float scale{}, radius{}, radians{};
};

struct BigEye;
struct Eye : EyeBase {
	static constexpr float cScale{ 1 };
	static constexpr float cRadius{ cScale * 8 };

	xx::FromTo<float> cDistanceRange{ 3.f, 6.f };
	xx::FromTo<float> cRadiansRange{ 3.f, 6.f };

	void Init(BigEye& owner_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_);
	void Init(Eye& prev_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_);

	void Update(BigEye& owner_);
	void Update(Eye& prev_);

	void Draw(Camera const& camera);
};

struct BigEye : EyeBase {
	static constexpr float cScale{ 30 };
	static constexpr float cRadius{ cScale * 8 };

	xx::Listi32<xx::Listi32<Eye>> eyess;

	void Init(XY const& pos_);

	void Update(Camera const& camera);

	void Draw(Camera const& camera);
};


struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	xx::Shared<BigEye> bigEye;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
