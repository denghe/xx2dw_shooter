#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"
#include "engine_shader.h"
#include "engine_shader_quad.h"
#include "engine_shader_linestrip.h"

#include "engine_rnd.h"
#include "engine_js_funcs.h"

// type same as EmscriptenKeyboardEvent.what
using KeyboardKeys_t = decltype(EmscriptenKeyboardEvent::which);
enum class KeyboardKeys : KeyboardKeys_t {
    Unknown = 0,
    A = 65,
    B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
    , MAX_VALUE
};

enum class MouseEvents : int {
    Unknown = 0, Down, Move, Up, Cancel
};

struct Mouse {
    XY pos{};
    std::array<bool, 16> btnStates{};
    //MouseEvents event{};
    //void* eventHandler{};
    //void ClearEvent() { event = MouseEvents::Unknown; }
    //void ClearEventAndSetHandler(void* h) { event = MouseEvents::Unknown; eventHandler = h; }
};

struct EngineBase : EngineBaseBase {

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext;

    Shader_QuadInstance shaderQuadInstance;
    Shader_LineStrip shaderLineStrip;
    // ... more

    Rnd rnd;

    std::array<bool, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysStates{};
    std::array<double, KeyboardKeys_t(KeyboardKeys::MAX_VALUE)> keyboardKeysDelays{};

    Mouse mouse;
    bool touchMode{};

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

    // example: gEngineBase->ShaderBegin(gEngineBase->shaderQuadInstance);
    template<typename ST>
    XX_FORCE_INLINE ST& ShaderBegin(ST& s) {
        if (shader != &s) {
            if (shader) {
                shader->End();
            }
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


    EngineBase();
};

inline EngineBase* gEngineBase{};

inline EngineBase::EngineBase() {
    gEngineBase = this;
}
