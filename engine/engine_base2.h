#pragma once
#include "engine_base1.h"
#include "engine_chartexcache.h"
#include "engine_tiledmap_sede.h"
#include "engine_fpsviewer.h"
#include "engine_linestrip.h"
#include "engine_dynamictexturepacker.h"
#include "engine_scale9sprite.h"

struct EngineBase2 : EngineBase1 {
    XX_FORCE_INLINE static EngineBase2& Instance() { return *(EngineBase2*)gEngine; }

    CharTexCache<24> ctcDefault;
    FpsViewer fpsViewer;
    bool showFps{ true };
};
