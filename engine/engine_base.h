#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"
#include "engine_shader.h"
#include "engine_shader_quad.h"
#include "engine_shader_linestrip.h"

#include "engine_rnd.h"
#include "engine_js_funcs.h"

struct EngineBase : EngineBase__ {

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;

    Shader_QuadInstance shaderQuadInstance;
    Shader_LineStrip shaderLineStrip;
    // ... more

    Rnd rnd;

    double nowSecs{}, delta{};
    double timePool{};
    int frameNumber{};
    EM_BOOL running{ EM_TRUE };

    void GLInit() {
        emscripten_set_canvas_element_size("canvas", (int)windowWidth, (int)windowHeight);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderQuadInstance.Init(this);
        shaderLineStrip.Init(this);
        // ... more
    }

    XX_FORCE_INLINE void GLViewport() {
        glViewport(0, 0, (int)windowWidth, (int)windowHeight);
    }

    XX_FORCE_INLINE void GLClear(RGBA8 c) {
        glClearColor(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glDepthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(false);
    }

    XX_FORCE_INLINE void GLUpdateBegin() {
        GLViewport();
        GLClear({});

        Shader::ClearCounter();
    }

    XX_FORCE_INLINE void GLUpdateEnd() {
        ShaderEnd();
    }

    XX_FORCE_INLINE void ShaderEnd() {
        if (shader) {
            shader->End();
        }
    }


    // todo: GLSetBlend == if diff GLShaderEnd + set blend + GLShaderBegin


    EngineBase();
};

inline EngineBase* gEngine{};

inline EngineBase::EngineBase() {
    gEngine = this;
}
