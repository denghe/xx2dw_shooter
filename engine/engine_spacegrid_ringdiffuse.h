#pragma once
#include "engine_includes.h"

struct CountRadius { int32_t count, radius; };

template<int32_t gridNumRows, int32_t gridCellDiameter>
struct SpaceGridRingDiffuseData {
    xx::List<CountRadius, int32_t> lens;
    xx::List<Vec2<int32_t>, int32_t> idxs;

    SpaceGridRingDiffuseData() {
        constexpr float step = gridCellDiameter / 2;
        lens.Emplace(1, 0);
        Vec2<int32_t> lastIdx{};
        idxs.Add(lastIdx);
        std::unordered_set<uint64_t> idxset;    // avoid duplicate
        for (int r = step; r < gridCellDiameter * gridNumRows; r += step) {
            auto c = 2 * M_PI * r;
            if (c < step) continue;
            auto lenBak = idxs.len;
            auto astep = M_PI * 2 * (step / c) / 10;
            for (float a = astep; a < M_PI * 2; a += astep) {
                XY pos{ r * cos(a), r * sin(a) };
                auto idx = (pos / gridCellDiameter).As<int32_t>();
                if (lastIdx != idx && idxset.insert((uint64_t&)idx).second) {
                    idxs.Add(idx);
                    lastIdx = idx;
                }
            }
            if (idxs.len > lenBak) {
                lens.Emplace(idxs.len, r);
            }
        }
    }
    SpaceGridRingDiffuseData(SpaceGridRingDiffuseData const&) = delete;
    SpaceGridRingDiffuseData& operator=(SpaceGridRingDiffuseData const&) = delete;
};
