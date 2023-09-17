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

struct FrameIndexBase {
	float frameIndex{};

	XX_FORCE_INLINE void ForwardFrame(float inc, float from, float to) {
		frameIndex += inc;
		if (frameIndex >= to) {
			frameIndex = from + (frameIndex - to);
		}
	}
	XX_FORCE_INLINE void ForwardFrame(float inc, float to) {
		ForwardFrame(inc, 0, to);
	}

	XX_FORCE_INLINE	void BackwardFrame(float inc, float from, float to) {
		frameIndex -= inc;
		if (frameIndex <= from) {
			frameIndex = to - (from - frameIndex);
		}
	}
	XX_FORCE_INLINE	void BackwardFrame(float inc, float to) {
		BackwardFrame(inc, 0, to);
	}
};

struct Hero : FrameIndexBase {
	constexpr static XY cAnchor{ 0.5f, 0.15f };
	constexpr static float cRadius{ 6.f };
	constexpr static std::array<float, 5> cFrameIndexRanges = { 0.f, 3.f, 6.f, 9.f, 12.f };
	constexpr static float cFrameInc{ 12.f / gDesign.fps };

	Quad body;

	XY pos;
	int heroId{};
	MoveDirections direction{};
	float frameIndexFrom{}, frameIndexTo{};
	float speed{1};

	void Init(int heroId_, XY bornPos);
	void SetDirection(MoveDirections d);
	void Draw();
	xx::Task<> mainLogic{ MainLogic() };
	xx::Task<> MainLogic();
};
