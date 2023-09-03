#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"
#include "engine_shader.h"
#include "engine_shader_quad.h"

#include "engine_rnd.h"
#include "engine_js_funcs.h"

struct EngineBase {
    inline static float w = 800, h = 600;          // can change at Init()
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;
    float flipY{ 1 };   // -1: flip  for ogl frame buffer
    Shader_QuadInstance shader;
    Rnd rnd;

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };

    void GLInit() {
        emscripten_set_canvas_element_size("canvas", (int)w, (int)h);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.Init();
    }

    void GLViewport() {
        glViewport(0, 0, (int)w, (int)h);
    }

    void GLClear(RGBA8 c) {
        glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glDepthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(false);
    }

    void GLUpdate() {
        GLViewport();
        GLClear({});

        Shader::ClearCounter();
        GLShaderBegin();
    }

    void GLShaderBegin() {
        shader.Begin(w, h * flipY);
    }

    void GLShaderEnd() {
        shader.End();
    }

    void GLUpdateEnd() {
        shader.End();
    }


    EngineBase();
};

inline EngineBase* gEngine{};

inline EngineBase::EngineBase() {
    gEngine = this;
}
