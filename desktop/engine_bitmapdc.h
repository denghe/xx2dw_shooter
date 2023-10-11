#pragma once
#include <engine_prims.h>

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

    void Init(HWND hWnd);

    ~BitmapDC();

    void removeCustomFont();

    bool setFont(std::string_view svFontName, int nSize = 0, bool enableBold = false);

    SIZE sizeWithText(std::wstring_view text, DWORD dwFmt, std::string_view fontName, int textSize, LONG nWidthLimit, LONG nHeightLimit, bool enableWrap, Overflow overflow);

    bool prepareBitmap(int nWidth, int nHeight);

    int drawText(std::string_view text, SIZE& tSize, TextAlign eAlign, std::string_view fontName, int textSize, bool enableWrap, Overflow overflow);

    xx::Data getTextureDataForText(std::string_view text, const FontDefinition& textDefinition, TextAlign align, int& width, int& height);
};
