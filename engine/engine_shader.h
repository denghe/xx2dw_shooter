#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"

struct Shader {
    inline static int drawVerts{}, drawCall{};
    inline static void ClearCounter() {
        drawVerts = {};
        drawCall = {};
    }

    GLShader v, f;
    GLProgram p;
    Shader() = default;
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
};
