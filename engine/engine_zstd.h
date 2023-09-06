#pragma once
#include "engine_includes.h"
#include "zstd.h"

inline void ZstdDecompress(std::string_view const& src, xx::Data& dst) {
    auto&& siz = ZSTD_getFrameContentSize(src.data(), src.size());
    if (ZSTD_CONTENTSIZE_UNKNOWN == siz) throw std::logic_error("ZstdDecompress error: unknown content size.");
    if (ZSTD_CONTENTSIZE_ERROR == siz) throw std::logic_error("ZstdDecompress read content size error.");
    dst.Resize(siz);
    if (0 == siz) return;
    siz = ZSTD_decompress(dst.buf, siz, src.data(), src.size());
    if (ZSTD_isError(siz)) throw std::logic_error("ZstdDecompress decompress error.");
    dst.Resize(siz);
}

inline void TryZstdDecompress(xx::Data& d) {
    if (d.len >= 4) {
        if (d[0] == 0x28 && d[1] == 0xB5 && d[2] == 0x2F && d[3] == 0xFD) {
            xx::Data d2;
            ZstdDecompress(d, d2);
            std::swap(d, d2);
        }
    }
}
