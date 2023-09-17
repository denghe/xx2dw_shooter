#pragma once
#include "game_looper_base.h"

struct Hero;

struct GameLooper : GameLooperBase<GameLooper> {
	void Init();
	void Update();
	xx::Task<> MainTask();
	void Draw();

	// flags
	bool ready{};

	// res
	std::vector<std::vector<xx::Shared<Frame>>> frames_heros;

	// objs
	xx::List<xx::Shared<Hero>, int32_t> heros;

	Camera camera;
};
extern GameLooper gLooper;

/*****************************************************************************************************/
/*****************************************************************************************************/

struct Hero {
	constexpr static XY cAnchor{ 0.5f, 0.15f };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };

	Quad body;

	XY pos;
	int heroId{};
	MoveDirections direction{};
	float frmaeIndex{}, frmaeIndexFrom{}, frmaeIndexTo{};

	void Init(int heroId_, XY bornPos);
	void SetDirection(MoveDirections d);
	void ForwardFrame();
	void BackwardFrame();
	void Draw();
	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic();
};
