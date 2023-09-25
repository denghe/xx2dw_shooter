#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"

struct Shader {
    static const size_t maxVertNums = 65535;	// 65535 for primitive restart index

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
    virtual ~Shader() {}
    virtual void Begin() = 0;
    virtual void End() = 0;
};
