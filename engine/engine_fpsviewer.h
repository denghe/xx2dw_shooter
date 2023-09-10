#pragma once
#include "engine_base.h"

struct FpsViewer {
    double lastSecs{}, drawCounter{}, updateCounter{}, fps{}, ups{};

    void Update() {
        ++updateCounter;
    }

    // CTC: char texture cache
    template<typename CTC>
    void Draw(CTC& ctc) {
        ++drawCounter;

        auto nowSecs = gEngine->nowSecs;
        if (auto elapsedSecs = nowSecs - lastSecs; elapsedSecs >= 1) {
            lastSecs = nowSecs;

            fps = drawCounter / elapsedSecs;
            drawCounter = 0;

            ups = updateCounter / elapsedSecs;
            updateCounter = 0;
        }

        gEngine->GLShaderEnd();
        auto s = std::string("FPS:") + std::to_string((uint32_t)fps)
                 + " UPS:" + std::to_string((uint32_t)ups)
                 + " DC:" + std::to_string(Shader::drawCall)
                 + " VC:" + std::to_string(Shader::drawVerts);
        gEngine->GLShaderBegin();

        ctc.Draw({ -gEngine->windowWidth_2, -gEngine->windowHeight_2 + ctc.canvasHeight_2 }, s);
    }
};
