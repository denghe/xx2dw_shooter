#pragma once
#include "engine_base.h"
#include "engine_zstd.h"
#include "engine_chartexcache.h"
#include "engine_texturepacker.h"
#include "engine_tiledmap_sede.h"
#include "engine_fpsviewer.h"
#include "engine_linestrip.h"
#include "engine_dynamictexturepacker.h"
#include "engine_curvemovepath.h"
#include "engine_spacegrid_box.h"
#include "engine_spacegrid_circle.h"
#include "engine_spacegrid_ringdiffuse.h"
#include "engine_camera.h"
#include "engine_gdesign.h"

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
