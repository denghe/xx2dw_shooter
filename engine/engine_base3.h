#pragma once
#include "engine_base2.h"
#include "engine_label.h"
#include "engine_button.h"

struct EngineBase3 : EngineBase2 {
	XX_FORCE_INLINE static EngineBase3& Instance() { return *(EngineBase3*)gEngine; }

};
