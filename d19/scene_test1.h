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
	static constexpr float cRadius{ cScale * 7 };

	xx::FromTo<float> cDistanceRange{};
	xx::FromTo<float> cRadiansRange{};

	void Init(EyeBase& prev_, xx::FromTo<float> const& cRadiansRange_, xx::FromTo<float> const& cDistanceRange_);

	void Update(EyeBase& prev_);

	void Draw(Camera const& camera);
};

struct BigEye : EyeBase {
	static constexpr float cScale{ 30 };
	static constexpr float cRadius{ cScale * 7 };
	static constexpr float cSpeed{ 2000.f / gDesign.fps };

	xx::Listi32<xx::Listi32<Eye>> eyess;

	void Init(XY const& pos_);

	void Update(Camera const& camera);

	void Draw(Camera const& camera);
};


struct SceneTest1 : Scene {
	xx::Shared<Node> rootNode;
	Camera camera;
	xx::Listi32<xx::Shared<BigEye>> bigEyes;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
