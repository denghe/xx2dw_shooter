#pragma once
#include <engine_framebuffer.h>

// sprite
struct Quad : QuadInstanceData {
    xx::Ref<Frame> frame;
    GLuint texId{};   // cache: == *frame->tex

    template<bool forceOverrideTexRectId = false>
    XX_FORCE_INLINE Quad& SetFrame(xx::Ref<Frame> f) {
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
        return *this;
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
    XX_FORCE_INLINE Quad& SetRotate(float r) {
        radians = r;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddRotate(float r) {
        radians += r;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetScale(XY const& s) {
        scale = s;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetScale(float s) {
        scale = { s, s };
        return *this;
    }
    XX_FORCE_INLINE Quad& AddScale(XY const& s) {
        scale += s;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddScale(float s) {
        scale += XY{ s, s };
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPosition(XY const& p) {
        pos = p;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPositionX(float x) {
        pos.x = x;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetPositionY(float y) {
        pos.y = y;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPosition(XY const& p) {
        pos += p;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPositionX(float x) {
        pos.x += x;
        return *this;
    }
    XX_FORCE_INLINE Quad& AddPositionY(float y) {
        pos.y += y;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColor(RGBA8 c) {
        color = c;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorA(uint8_t a) {
        color.a = a;
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorAf(float a) {
        color.a = uint8_t(255 * a);
        return *this;
    }
    XX_FORCE_INLINE Quad& SetColorplus(float v) {
        colorplus = v;
        return *this;
    }
    XX_FORCE_INLINE Quad& Draw() const {
        assert(texId);
        EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance).Draw(texId, *this);
        return (Quad&)*this;
    }
};
