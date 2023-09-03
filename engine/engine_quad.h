#pragma once
#include "engine_base.h"
#include "engine_framebuffer.h"
#include "engine_frame.h"

// sprite
struct Quad : QuadInstanceData {
    xx::Shared<GLTexture> tex;

    Quad& SetTexture(xx::Shared<GLTexture> const& t) {
        tex = t;
        texRectX = 0;
        texRectY = 0;
        texRectW = t->Width();
        texRectH = t->Height();
        return *this;
    }
    Quad& SetFrame(xx::Shared<Frame> const& f) {
        assert(f && !f->textureRotated);	// known issue: not support now
        tex = f->tex;
        texRectX = f->textureRect.x;
        texRectY = f->textureRect.y;
        texRectW = (uint16_t)f->textureRect.wh.x;
        texRectH = (uint16_t)f->textureRect.wh.y;
        return *this;
    }
    template<typename T = float>
    XY Size() const {
        assert(tex);
        return { (T)texRectW, (T)texRectH };
    }
    Quad& SetAnchor(XY const& a) {
        anchor = a;
        return *this;
    }
    Quad& SetRotate(float const& r) {
        radians = r;
        return *this;
    }
    Quad& AddRotate(float const& r) {
        radians += r;
        return *this;
    }
    Quad& SetScale(XY const& s) {
        scale = s;
        return *this;
    }
    Quad& SetScale(float const& s) {
        scale = { s, s };
        return *this;
    }
    Quad& SetPosition(XY const& p) {
        pos = p;
        return *this;
    }
    Quad& SetPositionX(float const& x) {
        pos.x = x;
        return *this;
    }
    Quad& SetPositionY(float const& y) {
        pos.y = y;
        return *this;
    }
    Quad& AddPosition(XY const& p) {
        pos += p;
        return *this;
    }
    Quad& AddPositionX(float const& x) {
        pos.x += x;
        return *this;
    }
    Quad& AddPositionY(float const& y) {
        pos.y += y;
        return *this;
    }
    Quad& SetColor(RGBA8 const& c) {
        color = c;
        return *this;
    }
    Quad& SetColorA(uint8_t const& a) {
        color.a = a;
        return *this;
    }
    Quad& SetColorAf(float const& a) {
        color.a = 255 * a;
        return *this;
    }
    Quad& Draw() {
        gEngine->shader.Draw(*tex, *this);
        return *this;
    }
};
