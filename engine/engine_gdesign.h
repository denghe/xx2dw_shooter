#pragma once
#include <engine_prims.h>

template<int width_, int height_, int fps_ = 60>
struct GDesign {
    static constexpr float fps = fps_;
    static constexpr float frameDelay = 1.f / fps_;

    static constexpr float width = width_;
    static constexpr float height = height_;

    static constexpr float width_2 = width / 2;
    static constexpr float height_2 = height / 2;

    static constexpr XY wh{ width, height };
    static constexpr XY wh_2{ width_2, height_2 };

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

    // m mean margin
    static constexpr float xym = 5.f;
    static constexpr XY xy1m { x1 + xym, y1 + xym };
    static constexpr XY xy2m { x2, y2 + xym };
    static constexpr XY xy3m { x3 - xym, y3 + xym };
    static constexpr XY xy4m { x4 + xym, y4 };
    static constexpr XY xy5m { x5, y5 };
    static constexpr XY xy6m { x6 - xym, y6 };
    static constexpr XY xy7m { x7 + xym, y7 - xym };
    static constexpr XY xy8m { x8, y8 - xym };
    static constexpr XY xy9m { x9 - xym, y9 - xym };

    // a mean anchoa
    static constexpr XY xy1a { 0, 0 };
    static constexpr XY xy2a { 0.5, 0 };
    static constexpr XY xy3a { 1, 0 };
    static constexpr XY xy4a { 0, 0.5 };
    static constexpr XY xy5a { 0.5, 0.5 };
    static constexpr XY xy6a { 1, 0.5 };
    static constexpr XY xy7a { 0, 1 };
    static constexpr XY xy8a { 0.5, 1 };
    static constexpr XY xy9a { 1, 1 };

    static constexpr float sqr2 = float(0.7071067811865475244);
};
