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
		xx::Listi32<ExplosionItem> items;
		void Init(int32_t cap = 10000);
		void Add(XY const& pos_, float radius, int32_t count = 256, float timeScale = 1.f);
		void Update();
		void Draw();
	};

}
