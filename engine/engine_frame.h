#pragma once
#include "engine_includes.h"
#include "engine_opengl.h"
#include "engine_prims.h"

// sprite frame
struct Frame {
    xx::Shared<GLTexture> tex;
    std::string key;
    // std::vector<std::string> aliases;	// unused
    std::optional<XY> anchor;
    XY spriteOffset{};
    XY spriteSize{};		// content size
    XY spriteSourceSize{};	// original pic size
    Rect textureRect{};
    size_t ud{};   // user data
    bool textureRotated{};
    std::vector<uint16_t> triangles;
    std::vector<float> vertices;
    std::vector<float> verticesUV;

    // single texture -> frame
    inline xx::Shared<Frame> static Create(xx::Shared<GLTexture> t) {
        auto f = xx::Make<Frame>();
        f->key = t->FileName();
        f->anchor = { 0.5, 0.5 };
        f->textureRotated = false;
        f->spriteSize = f->spriteSourceSize = { (float)t->Width(), (float)t->Height() };
        f->spriteOffset = {};
        f->textureRect = { 0, 0, f->spriteSize.x, f->spriteSize.y };
        f->tex = std::move(t);
        return f;
    }
};
