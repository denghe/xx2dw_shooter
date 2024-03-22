#pragma once
#include "looper.h"

namespace Effect {

	struct ExplosionItem {
		XY pos{}, inc{};
		float radians{};
		RGBA8 color{};
		int32_t beginFrameNumber{};
		int32_t endFrameNumber{};
	};

	struct ExplosionManager {
		int32_t* frameNumber{};
		Camera* camera{};
		Rnd* rnd{};
		xx::Ref<Frame> frame;
		RGBA8 color;
		xx::Listi32<ExplosionItem> items;

		void Init(int32_t* frameNumber_, Camera* camera_, Rnd* rnd_, xx::Ref<Frame> frame_, RGBA8 color_ = RGBA8_White, int32_t cap = 10000);
		void Add(XY const& pos_, float radius, int32_t count = 256, float timeScale = 1.f, bool rndRadians = false);
		void Update();
		void Draw();
	};

}
