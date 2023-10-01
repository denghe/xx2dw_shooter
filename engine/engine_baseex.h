#pragma once
#include "engine_base.h"
#include "engine_zstd.h"
#include "engine_chartexcache.h"
#include "engine_texturepacker.h"
#include "engine_tiledmap_sede.h"
#include "engine_fpsviewer.h"

struct EngineBaseEx : EngineBase {
    CharTexCache<24> ctcDefault;
    FpsViewer fpsViewer;
    bool showFps{ true };

    EngineBaseEx();
};

inline EngineBaseEx* gEngineBaseEx{};

inline EngineBaseEx::EngineBaseEx() {
    gEngineBaseEx = this;
}
