#pragma once
#include <engine_frames.h>

struct TexturePacker : Frames {
    bool premultiplyAlpha;
    std::string realTextureFileName;
    std::string plistUrl;

    // fill by plist file's binary content ( .blist ). success return 0
    int Load(xx::Data_r dr, std::string_view const& plistUrl_) {
        frames.clear();
        premultiplyAlpha = {};
        realTextureFileName.clear();
        plistUrl = plistUrl_;

        std::string rootPath;
        if (auto&& i = plistUrl.find_last_of("/"); i != plistUrl.npos) {
            rootPath = plistUrl.substr(0, i + 1);
        }

        size_t numFrames{};
        if (dr.len < 8 || memcmp(dr.buf, "blist_1 ", 8) != 0) return -1;                // file header verify
        if (int r = dr.Read(realTextureFileName, premultiplyAlpha, numFrames)) return r;
        for (size_t i = 0; i < numFrames; ++i) {
            auto f = xx::MakeRef<Frame>();
            if (int r = dr.Read(f->key, f->anchor)) return r;
            if (int r = dr.Read((xx::RWFloatInt16&)f->spriteOffset.x, (xx::RWFloatInt16&)f->spriteOffset.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->spriteSize.x, (xx::RWFloatUInt16&)f->spriteSize.y)) return r;
            if (int r = dr.Read((xx::RWFloatUInt16&)f->spriteSourceSize.x, (xx::RWFloatUInt16&)f->spriteSourceSize.y)) return r;
            if (int r = dr.ReadFixed(f->textureRect.x)) return r;
            if (int r = dr.ReadFixed(f->textureRect.y)) return r;
            if (int r = dr.ReadFixed(f->textureRect.w)) return r;
            if (int r = dr.ReadFixed(f->textureRect.h)) return r;
            //printf("load frame key = %s\n", f->key.c_str());
            frames.emplace_back(std::move(f));
        }
        realTextureFileName = rootPath + realTextureFileName;

        return 0;
    }
};
