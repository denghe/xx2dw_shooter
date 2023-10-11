#pragma once
#include <engine_framebuffer.h>

// sprite
struct Quad : QuadInstanceData {
    xx::Shared<Frame> frame;
    GLuint texId{};   // cache: == *frame->tex

    template<bool forceOverrideTexRectId = false>
    XX_FORCE_INLINE Quad& SetFrame(xx::Shared<Frame> f) {
        assert(f);
        assert(f->tex);
        if constexpr (!forceOverrideTexRectId) {
            if (frame == f) return *this;
        }
        texRect.data = f->textureRect.data;
        texId = f->tex->GetValue();
        frame = std::move(f);
        return *this;
    }

    XX_FORCE_INLINE Quad& ClearFrame() {
        frame.Reset();
        texId = 0;
    }

    template<typename T = float>
    XX_FORCE_INLINE XY Size() const {
        assert(frame);
        return { (T)texRect.w, (T)texRect.h };
    }
    XX_FORCE_INLINE Quad& SetAnchor(XY const& a) {
        anchor = a;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetRotate(float const& r) {
        radians = r;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddRotate(float const& r) {
        radians += r;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetScale(XY const& s) {
        scale = s;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetScale(float const& s) {
        scale = { s, s };
        return *this;
    }
    XX_FORCE_INLINE Quad& AddScale(XY const& s) {
        scale += s;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddScale(float const& s) {
        scale += XY{ s, s };
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPosition(XY const& p) {
        pos = p;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPositionX(float const& x) {
        pos.x = x;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPositionY(float const& y) {
        pos.y = y;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPosition(XY const& p) {
        pos += p;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPositionX(float const& x) {
        pos.x += x;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPositionY(float const& y) {
        pos.y += y;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColor(RGBA8 const& c) {
        color = c;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorA(uint8_t const& a) {
        color.a = a;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorAf(float const& a) {
        color.a = uint8_t(255 * a);
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorplus(float const& v) {
        colorplus = v;
        return *this;
    }
    XX_FORCE_INLINE Quad& Draw() const {
        assert(texId);
        EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance).Draw(texId, *this);
        return (Quad&)*this;
    }
};
