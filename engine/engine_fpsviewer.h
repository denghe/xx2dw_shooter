#pragma once
#include "engine_base.h"

struct FpsViewer {
    double lastSecs{}, counter{}, fps{};

    // CTC: char texture cache
    template<typename CTC>
    void Draw(CTC& cp) {
        ++counter;
        auto nowSecs = xx::NowSteadyEpochSeconds();
        if (auto elapsedSecs = nowSecs - lastSecs; elapsedSecs >= 1) {
            lastSecs = nowSecs;
            fps = counter / elapsedSecs;
            counter = 0;
        }

        gEngine->GLShaderEnd();
        auto s = std::string("FPS:") + std::to_string((uint32_t)fps)
                 + " DC:" + std::to_string(Shader::drawCall)
                 + " VC:" + std::to_string(Shader::drawVerts);
        gEngine->GLShaderBegin();

        cp.Draw({ -gEngine->w / 2, -gEngine->h / 2 + cp.canvasHeight / 2 }, s);
    }
};
