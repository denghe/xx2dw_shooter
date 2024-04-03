#pragma once
#include "looper.h"

namespace Test4 {

	struct Scene : ::Scene {
		xx::Task<> MainTask = MainTask_();
		xx::Task<> MainTask_();
		virtual void Draw() override;
	};

}
