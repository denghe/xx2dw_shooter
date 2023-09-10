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

    Vec2 MakeAdd(IsArithmetic auto const& vx, IsArithmetic auto const& vy) {
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
    auto MakeNormalize() -> Vec2<R> {
        auto v = std::sqrt(U(x * x + y * y));
        assert(v);
        return { R(x / v), R(y / v) };
    }

    Vec2& FlipY() {
        y = -y;
        return *this;
    }
    template<typename R = T>
    auto MakeFlipY() -> Vec2<R> {
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

// todo: more check funcs

// b: box    c: circle    w: width    h: height    r: radius
// if intersect return true
template<typename T = int32_t>
bool CheckBoxCircleIntersects(T const& bx, T const& by, T const& brw, T const& brh, T const& cx, T const& cy, T const& cr) {
    auto dx = std::abs(cx - bx);
    if (dx > brw + cr) return false;

    auto dy = std::abs(cy - by);
    if (dy > brh + cr) return false;

    if (dx <= brw || dy <= brh) return true;

    auto dx2 = dx - brw;
    auto dy2 = dy - brh;
    return dx2 * dx2 + dy2 * dy2 <= cr * cr;
}


// b: box    c: circle    w: width    h: height    r: radius
// if intersect, cx & cy will be changed & return true
template<typename T = int32_t>
bool MoveCircleIfIntersectsBox(T const& bx, T const& by, T const& brw, T const& brh, T& cx, T& cy, T const& cr) {
    auto dx = std::abs(cx - bx);
    if (dx > brw + cr) return false;

    auto dy = std::abs(cy - by);
    if (dy > brh + cr) return false;

    if (dx <= brw || dy <= brh) {
        if (brw - dx > brh - dy) {
            if (by > cy) {
                cy = by - brh - cr - 1;	// top
            } else {
                cy = by + brh + cr + 1;	// bottom
            }
        } else {
            if (bx > cx) {
                cx = bx - brw - cr - 1;	// left
            } else {
                cx = bx + brw + cr + 1;	// right
            }
        }
        return true;
    }

    auto dx2 = dx - brw;
    auto dy2 = dy - brh;
    if (dx2 * dx2 + dy2 * dy2 <= cr * cr) {
        // change cx & cy
        auto incX = dx2, incY = dy2;
        float dSeq = dx2 * dx2 + dy2 * dy2;
        if (dSeq == 0.0f) {
            incX = brw + cr * (1.f / 1.414213562373095f) + 1;
            incY = brh + cr * (1.f / 1.414213562373095f) + 1;
        } else {
            auto d = std::sqrt(dSeq);
            incX = brw + cr * dx2 / d + 1;
            incY = brh + cr * dy2 / d + 1;
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

    XY Apply(XY const& point) const {
        return { (float)((double)a * point.x + (double)c * point.y + tx), (float)((double)b * point.x + (double)d * point.y + ty) };
    }
};
