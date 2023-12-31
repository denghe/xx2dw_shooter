﻿#pragma once
#include <rect_pack_2d.h>
#include <engine_base1.h>
#include <engine_framebuffer.h>
#include <engine_quad.h>
#include <engine_frames.h>

template<int maxTextureWidthHeight_ = 4096>
struct DynamicTexturePacker : protected Frames {
    static constexpr int maxTextureWidthHeight = maxTextureWidthHeight_;

    void Clear() {
        frames.clear();
    }

    // need ogl frame env
    // return true: success
    template<bool newFrame = false>
    bool Fill(std::vector<xx::Ref<Frame>>& subFrames) {
        using namespace rect_pack_2d;
        frames.clear();
        frames.reserve(subFrames.size());
        std::vector<rect_xywhf> rects;
        std::vector<rect_xywhf*> rectptrs;
        rects.reserve(subFrames.size());
        rectptrs.reserve(subFrames.size());
        for (auto& sf : subFrames) {
            auto& r = rects.emplace_back(0, 0, sf->textureRect.w, sf->textureRect.h);
            r.ud = &sf;
            rectptrs.push_back(&r);
        }
        std::vector<bin> bins;
        if (!pack(rectptrs.data(), (int)rectptrs.size(), maxTextureWidthHeight, false, bins)) return false;

        FrameBuffer fb(true);
        for (auto& bin : bins) {
            Vec2<> wh{ xx::Round2n(bin.size.w), xx::Round2n(bin.size.h) };
            auto t = FrameBuffer::MakeTexture(wh.As<uint32_t>());
            XY basePos{ -wh.x / 2.f, wh.y / 2.f };
            Quad q;
            q.SetAnchor({ 0, 1 });
            fb.DrawTo(t, {}, [&]() {
                for (auto& r : bin.rects) {
                    auto& sf = *(xx::Ref<Frame>*)r->ud;
                    q.SetPosition(basePos + XY{ (float)r->x, (float)-r->y }).SetFrame(sf).Draw();
                }
            });
            for (auto& r : bin.rects) {
                auto& sf = *(xx::Ref<Frame>*)r->ud;
                if constexpr (newFrame) {
                    auto&& f = frames.emplace_back().Emplace();
                    *f = *sf;
                    f->textureRect.x = r->x;
                    f->textureRect.y = r->y;
                    f->tex = t;
                } else {
                    frames.emplace_back(sf);
                    sf->textureRect.x = r->x;
                    sf->textureRect.y = r->y;
                    sf->tex = t;
                }
            }
        }
        return true;
    }

    bool Fill(std::vector<xx::Ref<GLTexture>> const& subTexs) {
        std::vector<xx::Ref<Frame>> fs;
        fs.reserve(subTexs.size());
        for (auto& t : subTexs) {
            fs.emplace_back(Frame::Create(t));
        }
        return Fill(fs);
    }
};
