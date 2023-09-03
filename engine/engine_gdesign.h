#pragma once
#include "engine_prims.h"

template<int width_, int height_>
struct GDesign {
    static constexpr float width = width_;
    static constexpr float height = height_;
    static constexpr float width_2 = width / 2;
    static constexpr float height_2 = height / 2;
    /*
        screen design anchor point
       ┌───────────────────────────────┐
       │ 7             8             9 │
       │                               │
       │                               │
       │ 4             5             6 │
       │                               │
       │                               │
       │ 1             2             3 │
       └───────────────────────────────┘
    */
    static constexpr float x1 = -width_2;
    static constexpr float y1 = -height_2;
    static constexpr float x2 = 0.f;
    static constexpr float y2 = -height_2;
    static constexpr float x3 = width_2;
    static constexpr float y3 = -height_2;
    static constexpr float x4 = -width_2;
    static constexpr float y4 = 0.f;
    static constexpr float x5 = 0.f;
    static constexpr float y5 = 0.f;
    static constexpr float x6 = width_2;
    static constexpr float y6 = 0.f;
    static constexpr float x7 = -width_2;
    static constexpr float y7 = height_2;
    static constexpr float x8 = 0.f;
    static constexpr float y8 = height_2;
    static constexpr float x9 = width_2;
    static constexpr float y9 = height_2;

    static constexpr XY xy1{ x1, y1 };
    static constexpr XY xy2{ x2, y2 };
    static constexpr XY xy3{ x3, y3 };
    static constexpr XY xy4{ x4, y4 };
    static constexpr XY xy5{ x5, y5 };
    static constexpr XY xy6{ x6, y6 };
    static constexpr XY xy7{ x7, y7 };
    static constexpr XY xy8{ x8, y8 };
    static constexpr XY xy9{ x9, y9 };
};
