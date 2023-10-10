#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"
#include "engine_shader.h"
#include "engine_shader_quad.h"
#include "engine_shader_linestrip.h"
#include "engine_frame.h"
#include "engine_texturepacker.h"

#include "engine_rnd.h"
#include "engine_zstd.h"

#include "engine_base0.h"


struct EngineBase1 : EngineBase0 {
    XX_FORCE_INLINE static EngineBase1& Instance() { return *(EngineBase1*)gEngine; }

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;

    Shader_QuadInstance shaderQuadInstance;
    Shader_LineStrip shaderLineStrip;
    // ... more


    void GLInit() {
#ifdef __EMSCRIPTEN__
        emscripten_set_canvas_element_size("canvas", (int)windowWidth, (int)windowHeight);
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = 0;
        attrs.majorVersion = 2;
        glContext = emscripten_webgl_create_context("canvas", &attrs);
        xx_assert(glContext);
        emscripten_webgl_make_context_current(glContext);
#else

#endif

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

    // example: EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
    template<typename ST>
    XX_FORCE_INLINE ST& ShaderBegin(ST& s) {
        if (shader != &s) {
            ShaderEnd();
            s.Begin();
            shader = &s;
        }
        return s;
    }

    XX_FORCE_INLINE void ShaderEnd() {
        if (shader) {
            shader->End();
            shader = {};
        }
    }

    // todo: GLSetBlend == if diff GLShaderEnd + set blend + GLShaderBegin

};
