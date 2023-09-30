#pragma once
#include "engine_includes.h"

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

    Vec2 operator+(IsArithmetic auto const& v) const {
        return { T(x + v), T(y + v) };
    }
    Vec2 operator-(IsArithmetic auto const& v) const {
        return { T(x - v), T(y - v) };
    }
    Vec2 operator*(IsArithmetic auto const& v) const {
        return { T(x * v), T(y * v) };
    }
    Vec2 operator/(IsArithmetic auto const& v) const {
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

    Vec2& operator+=(IsArithmetic auto const& v) {
        x = T(x + v);
        y = T(y + v);
        return *this;
    }
    Vec2 operator-=(IsArithmetic auto const& v) {
        x = T(x - v);
        y = T(y - v);
        return *this;
    }
    Vec2& operator*=(IsArithmetic auto const& v) {
        x = T(x * v);
        y = T(y * v);
        return *this;
    }
    Vec2 operator/=(IsArithmetic auto const& v) {
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

    static Vec2 Make(IsArithmetic auto const& vx, IsArithmetic auto const& vy) {
        return { T(vx), T(vy) };
    }

    static Vec2 Make(IsArithmetic auto const& vxy) {
        return { T(vxy), T(vxy) };
    }

    Vec2 MakeAdd(HasFieldXY auto const& v) const {
        return { x + v.x, y + v.y };
    }

    Vec2 MakeAdd(IsArithmetic auto const& vx, IsArithmetic auto const& vy) const {
        return { x + vx, y + vy };
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
        return x == T{} && y == T{};
    }

    void Clear() {
        x = {};
        y = {};
    }

    template<typename U = float>
    Vec2& Normalize() {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        x = T(x / v);
        y = T(y / v);
        return *this;
    }
    template<typename R = T, typename U = float>
    auto MakeNormalize() const -> Vec2<R> {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
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

// pos
using XY = Vec2<float>;

// texture uv mapping pos
struct UV {
    uint16_t u, v;
};

// 4 bytes color
struct RGBA8 {
    uint8_t r, g, b, a;
    bool operator==(RGBA8 const&) const = default;
    bool operator!=(RGBA8 const&) const = default;
};

// 4 floats color
struct RGBA {
    float r, g, b, a;

    operator RGBA8() const {
        return { uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255), uint8_t(a * 255) };
    }

    RGBA operator+(RGBA const& v) const {
        return { r + v.r, g + v.g, b + v.b, a + v.a };
    }
    RGBA operator-(RGBA const& v) const {
        return { r - v.r, g - v.g, b - v.b, a - v.a };
    }

    RGBA operator*(IsArithmetic auto const& v) const {
        return { r * v, g * v, b * v, a * v };
    }
    RGBA operator/(IsArithmetic auto const& v) const {
        return { r / v, g / v, b / v, a / v };
    }

    RGBA& operator+=(RGBA const& v) {
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


struct Shader;
struct EngineBase__ {
    float windowWidth = 800, windowHeight = 600;          // can change at Init()
    float windowWidth_2 = windowWidth / 2, windowHeight_2 = windowHeight / 2;
    float flipY{ 1 };   // -1: flip  for ogl frame buffer
    Shader* shader{};
};


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
}


/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/


struct AffineTransform {
    float a, b, c, d;
    float tx, ty;

    inline static AffineTransform MakePosScaleRadiansAnchorSize(XY const& pos, XY const& scale, float const& radians, XY const& anchorSize) {
        auto x = pos.x;
        auto y = pos.y;
        float c = 1, s = 0;
        if (radians) {
            c = std::cos(-radians);
            s = std::sin(-radians);
        }
        if (!anchorSize.IsZero()) {
            x += c * scale.x * -anchorSize.x - s * scale.y * -anchorSize.y;
            y += s * scale.x * -anchorSize.x + c * scale.y * -anchorSize.y;
        }
        return { c * scale.x, s * scale.x, -s * scale.y, c * scale.y, x, y };
    }

    inline static AffineTransform MakePosScaleRadians(XY const& pos, XY const& scale, float const& radians) {
        auto x = pos.x;
        auto y = pos.y;
        float c = 1, s = 0;
        if (radians) {
            c = std::cos(-radians);
            s = std::sin(-radians);
        }
        return { c * scale.x, s * scale.x, -s * scale.y, c * scale.y, x, y };
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

    void Identity() {
        a = 1;
        b = 0;
        c = 0;
        d = 1;
        tx = 0;
        ty = 0;
    }

    XY Apply(XY const& point) const {
        return { (float)((double)a * point.x + (double)c * point.y + tx), (float)((double)b * point.x + (double)d * point.y + ty) };
    }

    AffineTransform MakeConcat(AffineTransform const& t2) {
        auto& t1 = *this;
        return { t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d,
            t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d,
            t1.tx * t2.a + t1.ty * t2.c + t2.tx,
            t1.tx * t2.b + t1.ty * t2.d + t2.ty };
    }
};


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

    // step change a to b by step. when a == b mean done
    inline XX_FORCE_INLINE bool Step(float& a, float b, float step) {
        assert(a >= -M_PI && a <= M_PI);
        assert(b >= -M_PI && b <= M_PI);
        assert(step <= M_PI);
        if ((b - a) * (b - a) > M_PI * M_PI) {
            if (b < a) {
                b += M_PI * 2;
            } else {
                b -= M_PI * 2;
            }
        }
        if (b > a) {
            if (b - a <= step) {
                a = b;
                return true;
            } else {
                a += step;
                if (a >= M_PI) {
                    a -= M_PI * 2;
                }
            }
        } else {
            if (a - b <= step) {
                a = b;
                return true;
            } else {
                a -= step;
                if (a <= -M_PI) {
                    a += M_PI * 2;
                }
            }
        }
        return false;
    }

}

/*******************************************************************************************************************************************/
/*******************************************************************************************************************************************/

namespace TranslateControl {

    // b: box    c: circle    w: width    h: height    r: radius
    // if intersect, cx & cy will be changed & return true
    template<typename T = int32_t>
    bool MoveCircleIfIntersectsBox(T const& bx, T const& by, T const& bHalfWidth, T const& bHalfHeight, T& cx, T& cy, T const& cr) {
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
            float dSeq = dx2 * dx2 + dy2 * dy2;
            if (dSeq == 0.0f) {
                incX = bHalfWidth + cr * (1.f / 1.414213562373095f) + 1;
                incY = bHalfHeight + cr * (1.f / 1.414213562373095f) + 1;
            } else {
                auto d = std::sqrt(dSeq);
                incX = bHalfWidth + cr * dx2 / d + 1;
                incY = bHalfHeight + cr * dy2 / d + 1;
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


    namespace Intersects {

        // b: box    c: circle    r: radius
        // if intersect return true
        template<typename T = int32_t>
        bool BoxCircle(T const& bx, T const& by, T const& bHalfWidth, T const& bHalfHeight, T const& cx, T const& cy, T const& cr) {
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
            for (int curr = 0, next = 1, e = vsEndIsFirst ? std::size(vs) - 1 : std::size(vs); curr < e; ++curr, ++next) {
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
            for (int curr = 0, next = 1, e = vsEndIsFirst ? std::size(vs) - 1 : std::size(vs); curr < e; ++curr, ++next) {
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

