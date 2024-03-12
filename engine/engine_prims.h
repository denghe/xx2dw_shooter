#pragma once
#include <engine_includes.h>

inline static constexpr float gPI{ (float)M_PI }, gNPI{ -gPI }, g2PI{ gPI * 2 };

template<typename T>
concept HasFieldXY = requires { T::x; T::y; };

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<T>;

template<typename T = int32_t>
struct Vec2 {
    using ElementType = T;
    T x, y;

    // -x
    Vec2 operator-() const {
        return { -x, -y };
    }

    // + - * /
    Vec2 operator+(HasFieldXY auto const& v) const {
        return { T(x + v.x), T(y + v.y) };
    }
    Vec2 operator-(HasFieldXY auto const& v) const {
        return { T(x - v.x), T(y - v.y) };
    }
    Vec2 operator*(HasFieldXY auto const& v) const {
        return { T(x * v.x), T(y * v.y) };
    }
    Vec2 operator/(HasFieldXY auto const& v) const {
        return { T(x / v.x), T(y / v.y) };
    }

    Vec2 operator+(IsArithmetic auto v) const {
        return { T(x + v), T(y + v) };
    }
    Vec2 operator-(IsArithmetic auto v) const {
        return { T(x - v), T(y - v) };
    }
    Vec2 operator*(IsArithmetic auto v) const {
        return { T(x * v), T(y * v) };
    }
    Vec2 operator/(IsArithmetic auto v) const {
        return { T(x / v), T(y / v) };
    }

    // += -= *= /=
    Vec2& operator+=(HasFieldXY auto const& v) {
        x = T(x + v.x);
        y = T(y + v.y);
        return *this;
    }
    Vec2& operator-=(HasFieldXY auto const& v) {
        x = T(x - v.x);
        y = T(y - v.y);
        return *this;
    }
    Vec2& operator*=(HasFieldXY auto const& v) {
        x = T(x * v.x);
        y = T(y * v.y);
        return *this;
    }
    Vec2& operator/=(HasFieldXY auto const& v) {
        x = T(x / v.x);
        y = T(y / v.y);
        return *this;
    }

    Vec2& operator+=(IsArithmetic auto v) {
        x = T(x + v);
        y = T(y + v);
        return *this;
    }
    Vec2 operator-=(IsArithmetic auto v) {
        x = T(x - v);
        y = T(y - v);
        return *this;
    }
    Vec2& operator*=(IsArithmetic auto v) {
        x = T(x * v);
        y = T(y * v);
        return *this;
    }
    Vec2 operator/=(IsArithmetic auto v) {
        x = T(x / v);
        y = T(y / v);
        return *this;
    }

    // == !=
    bool operator==(HasFieldXY auto const& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(HasFieldXY auto const& v) const {
        return x != v.x || y != v.y;
    }

    static Vec2 Make(IsArithmetic auto vx, IsArithmetic auto vy) {
        return { T(vx), T(vy) };
    }

    static Vec2 Make(IsArithmetic auto v) {
        return { T(v), T(v) };
    }

    Vec2 MakeAdd(HasFieldXY auto const& v) const {
        return { T(x + v.x), T(y + v.y) };
    }

    Vec2 MakeAdd(IsArithmetic auto vx, IsArithmetic auto vy) const {
        return { T(x + vx), T(y + vy) };
    }

    void Set(HasFieldXY auto const& v) {
        x = T(v.x);
        y = T(v.y);
    }

    template<typename U = float>
    auto As() const -> Vec2<U> {
        return { (U)x, (U)y };
    }

    bool IsZero() const {
        if constexpr (std::is_floating_point_v<T>) return (std::abs(x) < std::numeric_limits<T>::epsilon()) && (std::abs(y) < std::numeric_limits<T>::epsilon());
        else return x == T{} && y == T{};
    }

    bool IsZeroSimple() const {
        return x == T{} && y == T{};
    }

    void Clear() {
        x = {};
        y = {};
    }

    template<typename U = float>
    Vec2& Normalize() {
        auto v = std::sqrt(U(x * x + y * y));
        x = T(x / v);
        y = T(y / v);
        return *this;
    }
    template<typename R = T, typename U = float>
    auto MakeNormalize() const -> Vec2<R> {
        auto v = std::sqrt(U(x * x + y * y));
        return { R(x / v), R(y / v) };
    }

    Vec2& FlipY() {
        y = -y;
        return *this;
    }
    template<typename R = T>
    auto MakeFlipY() const -> Vec2<R> {
        return { R(x), R(-y) };
    }
};

template<typename T>
struct IsVec2 : std::false_type {};
template<typename T>
struct IsVec2<Vec2<T>> : std::true_type {};
template<typename T>
struct IsVec2<Vec2<T>&> : std::true_type {};
template<typename T>
struct IsVec2<Vec2<T> const&> : std::true_type {};
template<typename T>
constexpr bool IsVec2_v = IsVec2<T>::value;

namespace xx {
    template<typename T>
    struct DataFuncs<T, std::enable_if_t<IsVec2_v<T>>> {
        template<bool needReserve = true>
        static inline void Write(Data& d, T const& in) {
            d.Write<needReserve>(in.x, in.y);
        }
        static inline int Read(Data_r& d, T& out) {
            return d.Read(out.x, out.y);
        }
    };

    template<typename T>
    struct StringFuncs<Vec2<T>, void> {
        static inline void Append(std::string& s, Vec2<T> const& in) {
            ::xx::Append(s, in.x, ", ", in.y);
        }
    };
}

// pos
using XY = Vec2<float>;

// texture uv mapping pos
struct UV {
    uint16_t u, v;
};

// 3 bytes color
struct RGB8 {
    uint8_t r, g, b;
    bool operator==(RGB8 const&) const = default;
    bool operator!=(RGB8 const&) const = default;
};
constexpr static RGB8 RGB8_Zero{ 0,0,0 };
constexpr static RGB8 RGB8_Red{ 255,0,0 };
constexpr static RGB8 RGB8_Green{ 0,255,0 };
constexpr static RGB8 RGB8_Blue{ 0,0,255 };
constexpr static RGB8 RGB8_White{ 255,255,255 };
constexpr static RGB8 RGB8_Black{ 0,0,0 };

// 4 bytes color
struct RGBA8 {
    uint8_t r, g, b, a;
    bool operator==(RGBA8 const&) const = default;
    bool operator!=(RGBA8 const&) const = default;
};
constexpr static RGBA8 RGBA8_Zero{0,0,0,0};
constexpr static RGBA8 RGBA8_Red{255,0,0,255};
constexpr static RGBA8 RGBA8_Green{0,255,0,255};
constexpr static RGBA8 RGBA8_Blue{0,0,255,255};
constexpr static RGBA8 RGBA8_White{255,255,255,255};
constexpr static RGBA8 RGBA8_Black{0,0,0,255};

namespace xx {
    template<typename T>
    struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<RGBA8, T>>> {
        static inline void Append(std::string& s, RGBA8 const& in) {
            ::xx::Append(s, in.r, ", ", in.g, ", ", in.b, ", ", in.a);
        }
    };
}

// 4 floats color
struct RGBA {
    float r, g, b, a;

    operator RGBA8() const {
        return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
    }

    RGBA operator+(RGBA v) const {
        return { r + v.r, g + v.g, b + v.b, a + v.a };
    }
    RGBA operator-(RGBA v) const {
        return { r - v.r, g - v.g, b - v.b, a - v.a };
    }

    RGBA operator*(IsArithmetic auto v) const {
        return { r * v, g * v, b * v, a * v };
    }
    RGBA operator/(IsArithmetic auto v) const {
        return { r / v, g / v, b / v, a / v };
    }

    RGBA& operator+=(RGBA v) {
        r += v.r;
        g += v.g;
        b += v.b;
        a += v.a;
        return *this;
    }
};

// pos + size
struct Rect : XY {
    XY wh;
};


struct PosRadius {
    XY pos;
    float radius;
};


union UVRect {
    struct {
        uint16_t x, y, w, h;
    };
    uint64_t data;
};

namespace xx {
    template<typename T>
    struct StringFuncs<T, std::enable_if_t<std::is_same_v<UVRect, std::decay_t<T>>>> {
        static inline void Append(std::string& s, UVRect const& in) {
            ::xx::Append(s, in.x, ", ", in.y, ", ", in.w, ", ", in.h);
        }
    };
}


/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


struct AffineTransform {
    float a, b, c, d;
    float tx, ty;

    // anchorSize = anchor * size
    void PosScaleRadiansAnchorSize(XY const& pos, XY const& scale, float radians, XY const& anchorSize) {
        float c_ = 1, s_ = 0;
        if (radians) {
            c_ = std::cos(-radians);
            s_ = std::sin(-radians);
        }
        a = c_ * scale.x;
        b = s_ * scale.x;
        c = -s_ * scale.y;
        d = c_ * scale.y;
        tx = pos.x + c_ * scale.x * -anchorSize.x - s_ * scale.y * -anchorSize.y;
        ty = pos.y + s_ * scale.x * -anchorSize.x + c_ * scale.y * -anchorSize.y;
    }

    void PosScaleRadians(XY const& pos, XY const& scale, float radians) {
        float c_ = 1, s_ = 0;
        if (radians) {
            c_ = std::cos(-radians);
            s_ = std::sin(-radians);
        }
        a = c_ * scale.x;
        b = s_ * scale.x;
        c = -s_ * scale.y;
        d = c_ * scale.y;
        tx = pos.x;
        ty = pos.y;
    }

    // anchorSize = anchor * size
    void PosScaleAnchorSize(XY const& pos, XY const& scale, XY const& anchorSize) {
        a = scale.x;
        b = 0;
        c = 0;
        d = scale.y;
        tx = pos.x + scale.x * -anchorSize.x;
        ty = pos.y + scale.y * -anchorSize.y;
    }

    void PosScale(XY const& pos, XY const& scale) {
        a = scale.x;
        b = 0;
        c = 0;
        d = scale.y;
        tx = pos.x;
        ty = pos.y;
    }

    void Pos(XY const& pos) {
        a = 1;
        b = 0;
        c = 0;
        d = 1;
        tx = pos.x;
        ty = pos.y;
    }

    void Identity() {
        a = 1;
        b = 0;
        c = 0;
        d = 1;
        tx = 0;
        ty = 0;
    }

    // default apply
    XY operator()(XY const& point) const {
        return { (float)((double)a * point.x + (double)c * point.y + tx), (float)((double)b * point.x + (double)d * point.y + ty) };
    }

    XY NoRadiansApply(XY const& point) const {
        return { (float)((double)a * point.x + tx), (float)((double)d * point.y + ty) };
    }

    // child concat parent
    AffineTransform MakeConcat(AffineTransform const& t2) {
        auto& t1 = *this;
        return { t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d, t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d,
            t1.tx * t2.a + t1.ty * t2.c + t2.tx, t1.tx * t2.b + t1.ty * t2.d + t2.ty };
    }

    AffineTransform MakeInvert() {
        auto& t = *this;
        auto determinant = 1 / (t.a * t.d - t.b * t.c);
        return { determinant * t.d, -determinant * t.b, -determinant * t.c, determinant * t.a,
            determinant * (t.c * t.ty - t.d * t.tx), determinant * (t.b * t.tx - t.a * t.ty) };
    }

    inline static AffineTransform MakePosScaleRadiansAnchorSize(XY const& pos, XY const& scale, float radians, XY const& anchorSize) {
        AffineTransform at;
        at.PosScaleRadiansAnchorSize(pos, scale, radians, anchorSize);
        return at;
    }

    inline static AffineTransform MakePosScaleAnchorSize(XY const& pos, XY const& scale, XY const& anchorSize) {
        AffineTransform at;
        at.PosScaleAnchorSize(pos, scale, anchorSize);
        return at;
    }

    inline static AffineTransform MakePosScaleRadians(XY const& pos, XY const& scale, float radians) {
        AffineTransform at;
        at.PosScaleRadians(pos, scale, radians);
        return at;
    }

    inline static AffineTransform MakePosScale(XY const& pos, XY const& scale) {
        return { scale.x, 0, 0, scale.y, pos.x, pos.y };
    }

    inline static AffineTransform MakePos(XY const& pos) {
        return { 1.0, 0.0, 0.0, 1.0, pos.x, pos.y };
    }

    inline static AffineTransform MakeIdentity() {
        return { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };
    }

};

namespace xx {
    template<typename T>
    struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<AffineTransform, T>>> {
        static inline void Append(std::string& s, AffineTransform const& in) {
            ::xx::Append(s, in.a, ", ", in.b, ", ", in.c, ", ", in.d, ", ", in.tx, ", ", in.ty);
        }
    };
}


// without rotation support
struct SimpleAffineTransform {
    float a{ 1 }, d{ 1 };
    float tx{}, ty{};

    // anchorSize = anchor * size
    XX_FORCE_INLINE void PosScaleAnchorSize(XY const& pos, XY const& scale, XY const& anchorSize) {
        a = scale.x;
        d = scale.y;
        tx = pos.x - scale.x * anchorSize.x;
        ty = pos.y - scale.y * anchorSize.y;
    }

    XX_FORCE_INLINE void Identity() {
        a = 1;
        d = 1;
        tx = 0;
        ty = 0;
    }

    XX_FORCE_INLINE XY const& Offset() const {
        return (XY&)tx;
    }

    XX_FORCE_INLINE XY const& Scale() const {
        return (XY&)a;
    }

    // apply
    XX_FORCE_INLINE XY operator()(XY const& point) const {
        return { (float)((double)a * point.x + tx), (float)((double)d * point.y + ty) };
    }

    // child concat parent
    XX_FORCE_INLINE SimpleAffineTransform MakeConcat(SimpleAffineTransform const& t2) const {
        auto& t1 = *this;
        return { t1.a * t2.a, t1.d * t2.d, t1.tx * t2.a + t2.tx, t1.ty * t2.d + t2.ty };
    }

    XX_FORCE_INLINE SimpleAffineTransform MakeInvert() const {
        auto& t = *this;
        auto determinant = 1 / (t.a * t.d);
        return { determinant * t.d, determinant * t.a, determinant * (-t.d * t.tx), determinant * (-t.a * t.ty) };
    }

    XX_FORCE_INLINE static SimpleAffineTransform MakeIdentity() {
        return { 1.0, 1.0, 0.0, 0.0 };
    }

    XX_FORCE_INLINE static SimpleAffineTransform MakePosScaleAnchorSize(XY const& pos, XY const& scale, XY const& anchorSize) {
        SimpleAffineTransform t;
        t.PosScaleAnchorSize(pos, scale, anchorSize);
        return t;
    }
};

namespace xx {
    template<typename T>
    struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<SimpleAffineTransform, T>>> {
        static inline void Append(std::string& s, SimpleAffineTransform const& in) {
            ::xx::Append(s, in.a, ", ", in.d, ", ", in.tx, ", ", in.ty);
        }
    };
}


/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


namespace FrameControl {

    inline XX_FORCE_INLINE void Forward(float& frameIndex, float inc, float from, float to) {
        frameIndex += inc;
        if (frameIndex >= to) {
            frameIndex = from + (frameIndex - to);
        }
    }

    inline XX_FORCE_INLINE void Forward(float& frameIndex, float inc, float to) {
        Forward(frameIndex, inc, 0, to);
    }

    inline XX_FORCE_INLINE	void Backward(float& frameIndex, float inc, float from, float to) {
        frameIndex -= inc;
        if (frameIndex <= from) {
            frameIndex = to - (from - frameIndex);
        }
    }

    inline XX_FORCE_INLINE	void Backward(float& frameIndex, float inc, float to) {
        Backward(frameIndex, inc, 0, to);
    }

}


/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


namespace RotateControl {

    inline XX_FORCE_INLINE float Gap(float tar, float cur) {
        auto gap = cur - tar;
        if (gap > gPI) {
            gap -= g2PI;
        }
        if (gap < gNPI) {
            gap += g2PI;
        }
        return gap;
    }

    // calc cur to tar by rate
    // return cur's new value
    inline XX_FORCE_INLINE float LerpAngleByRate(float tar, float cur, float rate) {
        auto gap = Gap(tar, cur);
        return cur - gap * rate;
    }

    // calc cur to tar by fixed raidians
    // return cur's new value
    inline XX_FORCE_INLINE float LerpAngleByFixed(float tar, float cur, float a) {
        auto gap = Gap(tar, cur);
        if (gap < 0) {
            if (gap >= -a) return tar;
            else return cur + a;
        } else {
            if (gap <= a) return tar;
            else return cur - a;
        }
    }

    // change cur to tar by a( max value )
    // return true: cur == tar
    inline XX_FORCE_INLINE bool Step(float& cur, float tar, float a) {
        return (cur = LerpAngleByFixed(tar, cur, a)) == tar;
    }

    // limit a by from ~ to
    // no change: return false
    inline XX_FORCE_INLINE bool Limit(float& a, float from, float to) {
        // -PI ~~~~~~~~~~~~~~~~~~~~~~~~~ a ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PI
        assert(a >= gNPI && a <= gPI);
        // from ~~~~~~~~~~~~~~ a ~~~~~~~~~~~~~~~~ to
        if (a >= from && a <= to) return false;
        // from ~~~~~~~~~~~~~~~ -PI ~~~~~~~~~~~~~~~~~~ to ~~~~~~~~~~~~~~~ PI
        if (from < gNPI) {
            // ~~~~~~ from ~~~~~~~ -PI ~~~~~~ to ~~~~ a ~~~~ 0 ~~~~~~~~~~~~~ PI
            if (a < 0) {
                if (a - to < from + g2PI - a) {
                    a = to;
                } else {
                    a = from + g2PI;
                }
                // ~~~~~ d ~~~~~~ from ~~~~~~~ -PI ~~~~~~~~ to ~~~~~~~~ 0 ~~~~~~~ a ~~~~ PI
            } else {
                auto d = a - g2PI;
                if (d >= from && d <= to) return false;
                else {
                    if (from - d < a - to) {
                        a = from + g2PI;
                    } else {
                        a = to;
                    }
                }
            }
            // -PI ~~~~~~~~~~~~~~~ from ~~~~~~~~~~~~~~~~~~ PI ~~~~~~~~~~~~~~~ to
        } else if (to > gPI) {
            // -PI ~~~~~~~~~~~~~~~ 0 ~~~~~ a ~~~~~ from ~~~~~~ PI ~~~~~~~ to
            if (a > 0) {
                if (from - a < a - (to - g2PI)) {
                    a = from;
                } else {
                    a = to - g2PI;
                }
                // -PI ~~~~~~~ a ~~~~~~~~ 0 ~~~~~~~ from ~~~~~~ PI ~~~~~~~ to ~~~~~ d ~~~~~
            } else {
                auto d = a + g2PI;
                if (d >= from && d <= to) return false;
                else {
                    if (from - a < d - to) {
                        a = from;
                    } else {
                        a = to - g2PI;
                    }
                }
            }
        } else {
            // -PI ~~~~~ a ~~~~ from ~~~~~~~~~~~~~~~~~~ to ~~~~~~~~~~~ PI
            if (a < from) {
                if (to <= 0 || from - a < a - (to - g2PI)) {
                    a = from;
                } else {
                    a = to;
                }
                // -PI ~~~~~~~~~ from ~~~~~~~~~~~~~~~~~~ to ~~~~~ a ~~~~~~ PI
            } else {
                if (from > 0 || a - to < from + g2PI - a) {
                    a = to;
                } else {
                    a = from;
                }
            }
        }
        return true;
    }

}

/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/

namespace TranslateControl {

    // b: box    c: circle    w: width    h: height    r: radius
    // if intersect, cx & cy will be changed & return true
    template<typename T = int32_t>
    bool MoveCircleIfIntersectsBox(T bx, T by, T bHalfWidth, T bHalfHeight, T& cx, T& cy, T cr) {
        auto dx = std::abs(cx - bx);
        if (dx > bHalfWidth + cr) return false;

        auto dy = std::abs(cy - by);
        if (dy > bHalfHeight + cr) return false;

        if (dx <= bHalfWidth || dy <= bHalfHeight) {
            if (bHalfWidth - dx > bHalfHeight - dy) {
                if (by > cy) {
                    cy = by - bHalfHeight - cr - 1;	// top
                } else {
                    cy = by + bHalfHeight + cr + 1;	// bottom
                }
            } else {
                if (bx > cx) {
                    cx = bx - bHalfWidth - cr - 1;	// left
                } else {
                    cx = bx + bHalfWidth + cr + 1;	// right
                }
            }
            return true;
        }

        auto dx2 = dx - bHalfWidth;
        auto dy2 = dy - bHalfHeight;
        if (dx2 * dx2 + dy2 * dy2 <= cr * cr) {
            // change cx & cy
            auto incX = dx2, incY = dy2;
            auto dSeq = dx2 * dx2 + dy2 * dy2;
            if (dSeq == T{}) {
                incX = bHalfWidth + T(cr * 0.7071067811865475 + 1);
                incY = bHalfHeight + T(cr * 0.7071067811865475 + 1);
            } else {
                auto d = std::sqrt(dSeq);
                incX = bHalfWidth + T(cr * dx2 / d) + 1;
                incY = bHalfHeight + T(cr * dy2 / d) + 1;
            }

            if (cx < bx) {
                incX = -incX;
            }
            if (cy < by) {
                incY = -incY;
            }
            cx = bx + incX;
            cy = by + incY;

            return true;
        }
        return false;
    }

}

/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/

namespace Calc {

    inline XX_FORCE_INLINE float DistancePow2(float x1, float y1, float x2, float y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }

    inline XX_FORCE_INLINE float Distance(float x1, float y1, float x2, float y2) {
        return std::sqrt(DistancePow2(x1, y1, x2, y2));
    }

    inline XX_FORCE_INLINE float DistancePow2(XY const& p1, XY const& p2) {
        return DistancePow2(p1.x, p1.y, p2.x, p2.y);
    }

    inline XX_FORCE_INLINE float Distance(XY const& p1, XY const& p2) {
        return Distance(p1.x, p1.y, p2.x, p2.y);
    }

    inline XX_FORCE_INLINE float DistanceLimit(float d, float from, float to) {
        if (d < from) return from;
        else if (d > to) return to;
        else return d;
    }

    /*
        XY p, c;
        ...
        p2 = RotatePoint(c - p, 1.23) + c;
    */
    inline XX_FORCE_INLINE XY RotatePoint(XY const& d, float radians) {
        auto c = std::cos(radians);
        auto s = std::sin(radians);
        return { d.x * c - d.y * s, d.x * s + d.y * c };
    }

    // copy from cocos
    // Bezier cubic formula:
    //    ((1 - t) + t)3 = 1 
    // Expands to ...
    //   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1 
    inline XX_FORCE_INLINE float Bezierat(float a, float b, float c, float d, float t) {
        return (std::pow(1.f - t, 3.f) * a +
            3.f * t * (std::pow(1.f - t, 2.f)) * b +
            3.f * std::pow(t, 2.f) * (1.f - t) * c +
            std::pow(t, 3.f) * d);
    }

    namespace Intersects {

        // b: box
        template<typename XY1, typename XY2>
        XX_FORCE_INLINE bool BoxPoint(XY1 const& b1minXY, XY1 const& b1maxXY, XY2 const& p) {
            return !(b1maxXY.x < p.x || p.x < b1minXY.x || b1maxXY.y < p.y || p.y < b1minXY.y);
        }

        // b: box
        template<typename XY1, typename XY2>
        XX_FORCE_INLINE bool BoxBox(XY1 const& b1minXY, XY1 const& b1maxXY, XY2 const& b2minXY, XY2 const& b2maxXY) {
            return !(b1maxXY.x < b2minXY.x || b2maxXY.x < b1minXY.x || b1maxXY.y < b2minXY.y || b2maxXY.y < b1minXY.y);
        }

        // b: box    c: circle    r: radius
        // if intersect return true
        template<typename T = int32_t>
        bool BoxCircle(T bx, T by, T bHalfWidth, T bHalfHeight, T cx, T cy, T cr) {
            auto dx = std::abs(cx - bx);
            if (dx > bHalfWidth + cr) return false;

            auto dy = std::abs(cy - by);
            if (dy > bHalfHeight + cr) return false;

            if (dx <= bHalfWidth || dy <= bHalfHeight) return true;

            auto dx2 = dx - bHalfWidth;
            auto dy2 = dy - bHalfHeight;
            return dx2 * dx2 + dy2 * dy2 <= cr * cr;
        }

        // reference here:
        // http://www.jeffreythompson.org/collision-detection/poly-circle.php

        template<typename T = float>
        XX_FORCE_INLINE bool CircleCircle(T c1x, T c1y, T c1r, T c2x, T c2y, T c2r) {
            auto dx = c1x - c2x;
            auto dy = c1y - c2y;
            auto rr = c1r + c2r;
            return (dx * dx) + (dy * dy) <= rr * rr;
        }

        template<typename T = float>
        XX_FORCE_INLINE bool PointCircle(T px, T py, T cx, T cy, T r) {
            auto dx = px - cx;
            auto dy = py - cy;
            return (dx * dx) + (dy * dy) <= r * r;
        }

        inline XX_FORCE_INLINE bool LinePoint(float x1, float y1, float x2, float y2, float px, float py) {
            float d1 = Calc::Distance(px, py, x1, y1);
            float d2 = Calc::Distance(px, py, x2, y2);
            float lineLen = Calc::Distance(x1, y1, x2, y2);
            return d1 + d2 >= lineLen - 0.1 && d1 + d2 <= lineLen + 0.1;
        }

        inline bool LineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) {
            if (PointCircle(x1, y1, cx, cy, r) || PointCircle(x2, y2, cx, cy, r)) return true;
            float dx = x1 - x2;
            float dy = y1 - y2;
            float dot = (((cx - x1) * (x2 - x1)) + ((cy - y1) * (y2 - y1))) / ((dx * dx) + (dy * dy));
            float closestX = x1 + (dot * (x2 - x1));
            float closestY = y1 + (dot * (y2 - y1));
            if (!LinePoint(x1, y1, x2, y2, closestX, closestY)) return false;
            dx = closestX - cx;
            dy = closestY - cy;
            return (dx * dx) + (dy * dy) <= r * r;
        }

        template<bool vsEndIsFirst = false, typename Vecs>
        bool PolygonPoint(Vecs const& vs, float px, float py) {
            bool collision{};
            for (int curr = 0, next = 1, e = vsEndIsFirst ? (int)std::size(vs) - 1 : (int)std::size(vs); curr < e; ++curr, ++next) {
                if constexpr (!vsEndIsFirst) {
                    if (next == e) next = 0;
                }
                auto& vc = vs[curr];
                auto& vn = vs[next];
                if (((vc.y > py && vn.y < py) || (vc.y < py && vn.y > py)) &&
                    (px < (vn.x - vc.x) * (py - vc.y) / (vn.y - vc.y) + vc.x)) {
                    collision = !collision;
                }
            }
            return collision;
        }

        template<bool checkInside = true, bool vsEndIsFirst = true, typename Vecs>
        bool PolyCircle(Vecs const& vs, float cx, float cy, float r) {
            for (int curr = 0, next = 1, e = vsEndIsFirst ? (int)std::size(vs) - 1 : (int)std::size(vs); curr < e; ++curr, ++next) {
                if constexpr (!vsEndIsFirst) {
                    if (next == e) next = 0;
                }
                if (LineCircle(vs[curr].x, vs[curr].y, vs[next].x, vs[next].y, cx, cy, r)) return true;
            }
            if constexpr (checkInside) {
                if (PolygonPoint<vsEndIsFirst>(vs, cx, cy)) return true;
            }
            return false;
        }

    }
}

