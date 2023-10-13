#pragma once
#include <engine_base1.h>

enum class TextAlign {
    CENTER = 0x33, /** Horizontal center and vertical center. */
    TOP = 0x13, /** Horizontal center and vertical top. */
    TOP_RIGHT = 0x12, /** Horizontal right and vertical top. */
    RIGHT = 0x32, /** Horizontal right and vertical center. */
    BOTTOM_RIGHT = 0x22, /** Horizontal right and vertical bottom. */
    BOTTOM = 0x23, /** Horizontal center and vertical bottom. */
    BOTTOM_LEFT = 0x21, /** Horizontal left and vertical bottom. */
    LEFT = 0x31, /** Horizontal left and vertical center. */
    TOP_LEFT = 0x11, /** Horizontal left and vertical top. */
};

enum class Overflow {
    NONE,           // In NONE mode, the dimensions is (0,0) and the content size will change dynamically to fit the label.
    CLAMP,          // In CLAMP mode, when label content goes out of the bounding box, it will be clipped.
    SHRINK,         // In SHRINK mode, the font size will change dynamically to adapt the content size.
    RESIZE_HEIGHT   // In RESIZE_HEIGHT mode, you can only change the width of label and the height is changed automatically.
};

struct FontDefinition {
    std::string _fontName;
    int _fontSize{};
    XY _dimensions{};
    RGB8 _fontFillColor{ RGB8_Zero };
    uint8_t _fontAlpha{ 255 };
    bool _enableWrap{ true };
    Overflow _overflow{};
};

struct BitmapDC {

    HWND _wnd{};
    HFONT _font{};
    HDC _dc{};
    HBITMAP _bmp{};

    void Init(HWND hWnd) {
        _wnd = hWnd;
        _font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HDC hdc = GetDC(hWnd);
        _dc = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);
    }

    ~BitmapDC() {
        if (_bmp) {
            DeleteObject(_bmp);
            _bmp = {};
        }
        if (_dc) {
            DeleteDC(_dc);
            _dc = {};
        }
        removeCustomFont();
    }

    void removeCustomFont() {
        HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefFont != _font) {
            DeleteObject(_font);
            _font = hDefFont;
        }
    }

    bool setFont(std::string_view svFontName, int nSize = 0, bool enableBold = false) {
        std::wstring fontName;
        fontName.resize(svFontName.size());
        fontName.resize(MultiByteToWideChar(CP_UTF8, 0, svFontName.data(), (int)svFontName.size(), fontName.data(), (int)fontName.size()));

        HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        LOGFONTW tNewFont{}, tOldFont{};
        GetObjectW(hDefFont, sizeof(tNewFont), &tNewFont);

        if (!fontName.empty()) {
            tNewFont.lfCharSet = DEFAULT_CHARSET;
            wcscpy_s(tNewFont.lfFaceName, LF_FACESIZE, fontName.c_str());
        }
        if (nSize) {
            tNewFont.lfHeight = -nSize;
        }
        tNewFont.lfWeight = enableBold ? FW_BOLD : FW_NORMAL;
        GetObjectW(_font, sizeof(tOldFont), &tOldFont);

        if (tOldFont.lfHeight == tNewFont.lfHeight && tOldFont.lfWeight == tNewFont.lfWeight && 0 == wcscmp(tOldFont.lfFaceName, tNewFont.lfFaceName)) return true;

        tNewFont.lfQuality = ANTIALIASED_QUALITY;                           // disable Cleartype
        removeCustomFont();                                                 // delete old font before create new
        _font = CreateFontIndirectW(&tNewFont);                             // create new font
        if (!_font) {
            _font = hDefFont;                                               // create failed, use default font
            return false;
        }
        return true;
    }

    SIZE sizeWithText(std::wstring_view text, DWORD dwFmt, std::string_view fontName, int textSize, LONG nWidthLimit, LONG nHeightLimit, bool enableWrap, Overflow overflow) {
        if (text.empty()) return {};
        RECT rc{};
        DWORD dwCalcFmt = DT_CALCRECT | DT_NOPREFIX;
        if (!enableWrap) {
            dwCalcFmt |= DT_SINGLELINE;
        }
        if (nWidthLimit > 0) {
            rc.right = nWidthLimit;
            dwCalcFmt |= DT_WORDBREAK | (dwFmt & DT_CENTER) | (dwFmt & DT_RIGHT);
        }
        if (overflow == Overflow::SHRINK) {
            LONG actualWidth = nWidthLimit + 1;
            LONG actualHeight = nHeightLimit + 1;
            int newFontSize = textSize + 1;

            while (actualWidth > nWidthLimit || actualHeight > nHeightLimit) {
                if (newFontSize <= 0) break;
                this->setFont(fontName, newFontSize);

                HGDIOBJ hOld = SelectObject(_dc, _font);                    // use current font to measure text extent
                rc.right = nWidthLimit;
                DrawTextW(_dc, text.data(), (int)text.size(), &rc, dwCalcFmt);   // measure text size
                SelectObject(_dc, hOld);

                actualWidth = rc.right;
                actualHeight = rc.bottom;
                --newFontSize;
            }
        } else {
            HGDIOBJ hOld = SelectObject(_dc, _font);                        // use current font to measure text extent
            DrawTextW(_dc, text.data(), (int)text.size(), &rc, dwCalcFmt);       // measure text size
            SelectObject(_dc, hOld);
        }
        return { rc.right, rc.bottom };
    }

    bool prepareBitmap(int nWidth, int nHeight) {
        if (_bmp) {
            DeleteObject(_bmp);                                             // release bitmap
            _bmp = nullptr;
        }
        if (nWidth > 0 && nHeight > 0) {
            _bmp = CreateBitmap(nWidth, nHeight, 1, 32, nullptr);
            if (!_bmp) return false;
        }
        return true;
    }

    int drawText(std::string_view text, SIZE& tSize, TextAlign eAlign, std::string_view fontName, int textSize, bool enableWrap, Overflow overflow) {
        if (text.empty()) return 0;

        DWORD dwFmt = DT_WORDBREAK | DT_NOPREFIX;
        if (!enableWrap) {
            dwFmt |= DT_SINGLELINE;
        }
        DWORD dwHoriFlag = (int)eAlign & 0x0f;
        DWORD dwVertFlag = ((int)eAlign & 0xf0) >> 4;

        switch (dwHoriFlag) {
        case 1:  // left
            dwFmt |= DT_LEFT;
            break;
        case 2:  // right
            dwFmt |= DT_RIGHT;
            break;
        case 3:  // center
            dwFmt |= DT_CENTER;
            break;
        }

        std::wstring ws;
        ws.resize(text.size());
        ws.resize(MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), ws.data(), (int)ws.size()));

        SIZE newSize = sizeWithText(ws, dwFmt, fontName, textSize, tSize.cx, tSize.cy, enableWrap, overflow);
        RECT rcText{};
        if (tSize.cx <= 0) {                                        // if content width is 0, use text size as content size
            tSize = newSize;
            rcText.right = newSize.cx;
            rcText.bottom = newSize.cy;
        } else {
            LONG offsetX{}, offsetY{};
            rcText.right = newSize.cx;          // store the text width to rectangle

            // calculate text horizontal offset. text isn't align to left and text's width less then content width,  then need adjust offset of X.
            if (1 != dwHoriFlag && newSize.cx < tSize.cx) {
                offsetX = (2 == dwHoriFlag) ? tSize.cx - newSize.cx : (tSize.cx - newSize.cx) / 2;      // align to right : align to center
            }

            // if content height is 0, use text height as content height 
            // else if content height less than text height, use content height to draw text
            if (tSize.cy <= 0) {
                tSize.cy = newSize.cy;
                dwFmt |= DT_NOCLIP;
                rcText.bottom = newSize.cy;     // store the text height to rectangle
            } else if (tSize.cy < newSize.cy) {
                rcText.bottom = tSize.cy;       // content height larger than text height need, clip text to rect
            } else {
                rcText.bottom = newSize.cy;     // store the text height to rectangle
                dwFmt |= DT_NOCLIP;             // content larger than text, need adjust vertical position

                // calculate text vertical offset
                offsetY = (2 == dwVertFlag) ? tSize.cy - newSize.cy         // align to bottom
                    : (3 == dwVertFlag) ? (tSize.cy - newSize.cy) / 2       // align to middle
                    : 0;                                                    // align to top
            }

            if (offsetX || offsetY) {
                OffsetRect(&rcText, offsetX, offsetY);
            }
        }

        if (!prepareBitmap(tSize.cx, tSize.cy)) return 0;

        // draw text
        HGDIOBJ hOldFont = SelectObject(_dc, _font);
        HGDIOBJ hOldBmp = SelectObject(_dc, _bmp);

        SetBkMode(_dc, TRANSPARENT);
        SetTextColor(_dc, RGB(255, 255, 255));  // white color

        // draw text
        auto nRet = DrawTextW(_dc, ws.data(), (int)ws.size(), &rcText, dwFmt);
        SelectObject(_dc, hOldBmp);
        SelectObject(_dc, hOldFont);
        return nRet;
    }

    xx::Data getTextureDataForText(std::string_view text, const FontDefinition& textDefinition, TextAlign align, int& width, int& height) {
        if (!setFont(textDefinition._fontName, (int)textDefinition._fontSize, false)) {
            xx::CoutN("Can't found font( ", textDefinition._fontName, " ), use system default");
        }

        // draw text. does changing to SIZE here affects the font size by rounding from float?
        SIZE size = { (LONG)textDefinition._dimensions.x, (LONG)textDefinition._dimensions.y };
        if (!drawText(text, size, align, textDefinition._fontName, textDefinition._fontSize, textDefinition._enableWrap, textDefinition._overflow)) return {};

        xx::Data d;
        d.Resize(size.cx * size.cy * 4);

        struct {
            BITMAPINFOHEADER bmiHeader;
            int mask[4];
        } bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        if (!GetDIBits(_dc, _bmp, 0, 0, nullptr, (LPBITMAPINFO)&bi, DIB_RGB_COLORS)) return {};

        width = (short)size.cx;
        height = (short)size.cy;

        // copy pixel data
        bi.bmiHeader.biHeight = (bi.bmiHeader.biHeight > 0) ? -bi.bmiHeader.biHeight : bi.bmiHeader.biHeight;
        GetDIBits(_dc, _bmp, 0, height, d.buf, (LPBITMAPINFO)&bi, DIB_RGB_COLORS);

        COLORREF textColor = (textDefinition._fontFillColor.b << 16 | textDefinition._fontFillColor.g << 8 | textDefinition._fontFillColor.r) & 0x00ffffff;
        float alpha = textDefinition._fontAlpha / 255.0f;
        COLORREF* pPixel = nullptr;
        for (int y = 0; y < height; ++y) {
            pPixel = (COLORREF*)d.buf + y * width;
            for (int x = 0; x < width; ++x) {
                COLORREF& clr = *pPixel;
                clr = ((BYTE)(GetRValue(clr) * alpha) << 24) | textColor;
                ++pPixel;
            }
        }

        // to rgba( rgb == ffffff, a keep )
        for (int i = 0, e = (int)d.len / 4; i < e; ++i) {
            auto& v = ((uint32_t*)d.buf)[i];
            v |= (0xffffff | (GetRValue(v) << 24));
        }
        return d;
    }

    inline static std::unique_ptr<BitmapDC> instance;
};

inline void init_gCanvas(int charSize, int width, int height, const char* font) {
    if (!BitmapDC::instance) {
        BitmapDC::instance = std::make_unique<BitmapDC>();
        BitmapDC::instance->Init(glfwGetWin32Window(EngineBase1::Instance().wnd));
    }
}

inline std::pair<float, float> upload_unicode_char_to_texture(int charSize, char const* buf) {
    int w{}, h{};
    auto d = BitmapDC::instance->getTextureDataForText(buf, { "Arial", 24 }, TextAlign::LEFT, w, h);
    if (w > 0) {
        GLTexParameteri();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d.buf);
    }
    CheckGLError();
    return { (float)w, (float)h };
}
