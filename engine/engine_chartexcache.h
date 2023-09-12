#pragma once
#include "engine_base.h"
#include "engine_framebuffer.h"
#include "engine_quad.h"

struct CharInfo {
    xx::Shared<GLTexture> tex;
    uint16_t texRectX{}, texRectY{}, texRectW{}, texRectH{};
};

template<int charSize_ = 24, int canvasWidth_ = int(charSize_ / 0.75), int canvasHeight_ = int(charSize_ / 0.75), int texWidth_ = 2048, int texHeight_ = 2048>
struct CharTexCache {
    static constexpr int charSize = charSize_, canvasWidth = canvasWidth_, canvasHeight = canvasHeight_, texWidth = texWidth_, texHeight = texHeight_;
    static constexpr int canvasWidth_2 = canvasWidth / 2, canvasHeight_2 = canvasHeight / 2;
    std::vector<xx::Shared<GLTexture>> texs;
    Quad cq;
    FrameBuffer fb;
    float cw{};
    XY p{ 0, texHeight - 1 };

    std::array<CharInfo, 256> bases;
    std::unordered_map<char32_t, CharInfo> extras;

    // need ogl frame env
    void Init(char const* font = "Arial") {
        fb.Init();
        init_gCanvas(charSize, canvasWidth, canvasHeight, font);

        texs.emplace_back(FrameBuffer::MakeTexture(Vec2{ texWidth, texHeight }));
        auto ct = xx::Make<GLTexture>(GLGenTextures<true>(), canvasWidth, canvasHeight, "");
        cq.SetFrame(Frame::Create(std::move(ct))).SetAnchor({ 0, 1 });

        char buf[16];
        for (char32_t c = 0; c < 256; ++c) {
            MakeCharInfo(c);
        }
    }

    void FillCharTex(char32_t c) {
        char buf[16];
        buf[xx::Char32ToUtf8(c, buf)] = '\0';
        glBindTexture(GL_TEXTURE_2D, cq.texId);
        cw = upload_unicode_char_to_texture(charSize, buf);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    CharInfo& MakeCharInfo(char32_t c) {
        FillCharTex(c);
        CharInfo* ci{};
        if (c < 256) {
            ci = &bases[c];
        } else {
            auto rtv = extras.insert(std::make_pair(c, CharInfo{}));
            ci = &rtv.first->second;
        }

        auto cp = p;
        if (p.x + cw > texWidth) {                  // line wrap
            cp.x = 0;
            p.x = cw;
            if (p.y - canvasHeight < 0) {                     // new page
                texs.emplace_back(FrameBuffer::MakeTexture(Vec2{ texWidth, texHeight }));
                p.y = cp.y = texHeight - 1;
            } else {                                // new line
                p.y -= canvasHeight;
                cp.y = p.y;
            }
        } else {                                    // current line
            p.x += cw;
        }

        auto& t = texs.back();
        fb.DrawTo(t, {}, [&]() {
            cq.SetPosition(cp + XY{ -texWidth / 2, -texHeight / 2 }).Draw();
        });

        ci->tex = t;
        ci->texRectX = cp.x;
        ci->texRectY = texHeight - 1 - cp.y;        // flip y for uv
        ci->texRectW = cw;
        ci->texRectH = canvasHeight;
        return *ci;
    }

    CharInfo& Find(char32_t c) {
        if (c < 256) {
            return bases[c];
        } else {
            if (auto iter = extras.find(c); iter != extras.end()) {
                return iter->second;
            } else {
                return MakeCharInfo(c);
            }
        }
    }

    // anchor: {0, 0.5}   todo: anchor, max width limit ?
    void Draw(XY pos, std::u32string_view const& s, RGBA8 color = { 255,255,255,255 }) {
        Quad q;
        q.SetAnchor({ 0.f, 0.5f });

        // make sure all char texture exists ( avoid framebuffer incomplete issue )
        auto cis = (CharInfo**)alloca(s.size() * sizeof(void*));
        auto e = s.size();
        for (size_t i = 0; i < e; ++i) {
            cis[i] = &Find(s[i]);
        }

        for (size_t i = 0; i < e; ++i) {
            auto&& ci = *cis[i];
            q.texId = ci.tex->GetValue();
            q.color = color;
            q.texRectX = ci.texRectX;
            q.texRectY = ci.texRectY;
            q.texRectW = ci.texRectW;
            q.texRectH = ci.texRectH;
            q.SetPosition(pos).Draw();
            pos.x += ci.texRectW;
        }
    }

    void Draw(XY const& pos, std::string_view const& s, RGBA8 color = {255,255,255,255}) {
        Draw(pos, xx::StringU8ToU32(s), color);
    }

    float Measure(std::u32string_view const& s) {
        float w{};
        for (size_t i = 0; i < s.size(); ++i) {
            w += Find(s[i]).texRectW;
        }
        return w;
    }

    float Measure(std::string_view const& s) {
        return Measure(xx::StringU8ToU32(s));
    }

};

