#pragma once
#include "rect_pack_2d.h"
#include "engine_base.h"
#include "engine_framebuffer.h"
#include "engine_frame.h"
#include "engine_quad.h"
#include "engine_frames.h"

template<int texWH_ = 2048>
struct DynamicTexturePacker : Frames {
    static constexpr int texWH = texWH_;

    void Clear() {
        frames.clear();
    }

    // need ogl frame env
    // return true: success
    template<bool newFrame = false>
    bool Fill(std::vector<xx::Shared<Frame>>& subFrames) {
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
        if (!pack(rectptrs.data(), rectptrs.size(), texWH, false, bins)) return false;

        FrameBuffer fb(true);
        for (auto& bin : bins) {
            int w = xx::Round2n(bin.size.w);
            int h = xx::Round2n(bin.size.h);
            auto t = FrameBuffer::MakeTexture(Vec2{ w, h });
            XY basePos{ -w / 2, h / 2 };
            fb.DrawTo(t, {}, [&]() {
                Quad q;
                q.SetAnchor({ 0, 1 });
                for (auto& r : bin.rects) {
                    auto& sf = *(xx::Shared<Frame>*)r->ud;
                    q.SetPosition(basePos + XY{ r->x, -r->y }).SetFrame(sf).Draw();
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
            });
        }
        return true;
    }

    bool Fill(std::vector<xx::Shared<GLTexture>> const& subTexs) {
        std::vector<xx::Shared<Frame>> fs;
        fs.reserve(subTexs.size());
        for (auto& t : subTexs) {
            fs.emplace_back(Frame::Create(t));
        }
        return Fill(fs);
    }
};
