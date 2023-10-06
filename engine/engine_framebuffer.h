#pragma once
#include "engine_base1.h"

struct FrameBuffer {
    GLFrameBuffer fb;
    XY bak{};

    // need ogl frame env
    explicit FrameBuffer(bool autoInit = false) {
        if (autoInit) {
            Init();
        }
    }

    // need ogl frame env
    FrameBuffer& Init() {
        if (fb) {
            throw std::logic_error("excessive initializations ?");
        }
        fb = MakeGLFrameBuffer();
        return *this;
    }

    template<typename WH>
    inline static xx::Shared<GLTexture> MakeTexture(WH const& wh, bool const& hasAlpha = true) {
        return xx::Make<GLTexture>(GLTexture::Create(wh.x, wh.y, hasAlpha));
    }

    template<typename Func>
    void DrawTo(xx::Shared<GLTexture>& t, std::optional<RGBA8> const& c, Func&& func) {
        Begin(t, c);
        func();
        End();
    }

    template<typename Func>
    xx::Shared<GLTexture> Draw(Vec2<uint32_t> const& wh, bool const& hasAlpha, std::optional<RGBA8> const& c, Func&& func) {
        auto t = MakeTexture(wh, hasAlpha);
        DrawTo(t, c, std::forward<Func>(func));
        return t;
    }

protected:
    void Begin(xx::Shared<GLTexture>& t, std::optional<RGBA8> const& c = {}) {
        auto& ge = EngineBase1::Instance();
        ge.ShaderEnd();
        bak.x = std::exchange(ge.windowWidth, t->Width());
        bak.y = std::exchange(ge.windowHeight, t->Height());
        ge.flipY = -1;
        BindGLFrameBufferTexture(fb, *t);
        ge.GLViewport();
        if (c.has_value()) {
            ge.GLClear(c.value());
        }
    }
    void End() {
        auto& ge = EngineBase1::Instance();
        ge.ShaderEnd();
        UnbindGLFrameBuffer();
        ge.windowWidth = bak.x;
        ge.windowHeight = bak.y;
        ge.flipY = 1;
        ge.GLViewport();
    }
};
