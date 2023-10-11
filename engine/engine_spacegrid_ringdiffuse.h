#pragma once
#include <engine_prims.h>

struct CountRadius { int32_t count; float radius; };

struct SpaceGridRingDiffuseData {
    xx::List<CountRadius, int32_t> lens;
    xx::List<Vec2<int32_t>, int32_t> idxs;

    void Init(int32_t gridNumRows, int32_t gridCellDiameter) {
        auto step = (float)gridCellDiameter / 2;
        lens.Emplace(0, 0.f);
        Vec2<int32_t> lastIdx{};
        idxs.Add(lastIdx);
        std::unordered_set<uint64_t> idxset;    // avoid duplicate
        for (float r = step; r < gridCellDiameter * gridNumRows; r += step) {
            auto c = 2 * M_PI * r;
            if (c < step) continue;
            auto lenBak = idxs.len;
            auto astep = float(M_PI * 2 * (step / c) / 10);
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
};
