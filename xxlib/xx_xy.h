#pragma once
#include "xx_string.h"

namespace xx {
    template<typename T>
    concept HasField_XY = requires { T::x; T::y; };

    template<typename T>
    concept IsArithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    struct X_Y {
        using ElementType = T;
        T x, y;

        constexpr X_Y() = default;
        constexpr X_Y(X_Y const&) = default;
        constexpr X_Y& operator=(X_Y const&) = default;

        constexpr X_Y(HasField_XY auto const& v) : x(T(v.x)), y(T(v.y)) {}
        constexpr X_Y(IsArithmetic auto x_, IsArithmetic auto y_) : x(T(x_)), y(T(y_)) {}
        constexpr X_Y(IsArithmetic auto v) : x(T(v)), y(T(v)) {}

        constexpr X_Y& operator=(HasField_XY auto const& v) { x = T(v.x); y = T(v.y); return *this; }
        constexpr X_Y& operator=(IsArithmetic auto v) { x = T(v); y = T(v); return *this; }

        constexpr X_Y operator-() const { return { -x, -y }; }

        constexpr X_Y operator+(HasField_XY auto const& v) const { return { T(x + v.x), T(y + v.y) }; }
        constexpr X_Y operator-(HasField_XY auto const& v) const { return { T(x - v.x), T(y - v.y) }; }
        constexpr X_Y operator*(HasField_XY auto const& v) const { return { T(x * v.x), T(y * v.y) }; }
        constexpr X_Y operator/(HasField_XY auto const& v) const { return { T(x / v.x), T(y / v.y) }; }

        constexpr X_Y operator+(IsArithmetic auto v) const { return { T(x + v), T(y + v) }; }
        constexpr X_Y operator-(IsArithmetic auto v) const { return { T(x - v), T(y - v) }; }
        constexpr X_Y operator*(IsArithmetic auto v) const { return { T(x * v), T(y * v) }; }
        constexpr X_Y operator/(IsArithmetic auto v) const { return { T(x / v), T(y / v) }; }

        constexpr X_Y& operator+=(HasField_XY auto const& v) { x = T(x + v.x); y = T(y + v.y); return *this; }
        constexpr X_Y& operator-=(HasField_XY auto const& v) { x = T(x - v.x); y = T(y - v.y); return *this; }
        constexpr X_Y& operator*=(HasField_XY auto const& v) { x = T(x * v.x); y = T(y * v.y); return *this; }
        constexpr X_Y& operator/=(HasField_XY auto const& v) { x = T(x / v.x); y = T(y / v.y); return *this; }

        constexpr X_Y& operator+=(IsArithmetic auto v) { x = T(x + v); y = T(y + v); return *this; }
        constexpr X_Y& operator-=(IsArithmetic auto v) { x = T(x - v); y = T(y - v); return *this; }
        constexpr X_Y& operator*=(IsArithmetic auto v) { x = T(x * v); y = T(y * v); return *this; }
        constexpr X_Y& operator/=(IsArithmetic auto v) { x = T(x / v); y = T(y / v); return *this; }

        constexpr bool operator==(HasField_XY auto const& v) const { return memcmp(this, &v, sizeof(v)) == 0; }
        constexpr bool operator!=(HasField_XY auto const& v) const { return memcmp(this, &v, sizeof(v)) != 0; }

        constexpr void Reset() {
            x = {};
            y = {};
        }

        template<typename U>
        constexpr auto As() const -> X_Y<U> {
            return { (U)x, (U)y };
        }

        constexpr bool IsZero() const {
            if constexpr (std::is_floating_point_v<T>) {
                return (std::abs(x) < std::numeric_limits<T>::epsilon())
                    && (std::abs(y) < std::numeric_limits<T>::epsilon());
            }
            else return x == T{} && y == T{};
        }

        constexpr bool IsZeroSimple() const {
            return x == T{} && y == T{};
        }

        template<typename R = T>
        constexpr auto FlipY() const -> X_Y<R> {
            return { R(x), R(-y) };
        }

        template<typename R = T, typename U = float>
        constexpr auto Mag2() const -> R {
            return R(x) * R(x) + R(y) * R(y);
        }

        template<typename R = T>
        constexpr auto Mag() const -> R {
            return (R)std::sqrt(Mag2<R>());
        }

        template<typename R = T>
        constexpr auto Normalize() const -> X_Y<R> {
            auto mag = Mag<R>();
            return { R(x) / mag, R(y) / mag };
        }

        // ...
    };


    template<typename T> inline X_Y<T> operator+(IsArithmetic auto const& v, X_Y<T> const& o) { return X_Y<T>{ v + o.x, v + o.y }; }
    template<typename T> inline X_Y<T> operator-(IsArithmetic auto const& v, X_Y<T> const& o) { return X_Y<T>{ v - o.x, v - o.y }; }
    template<typename T> inline X_Y<T> operator*(IsArithmetic auto const& v, X_Y<T> const& o) { return X_Y<T>{ v * o.x, v * o.y }; }
    template<typename T> inline X_Y<T> operator/(IsArithmetic auto const& v, X_Y<T> const& o) { return X_Y<T>{ v / o.x, v / o.y }; }

    using XYi = X_Y<int32_t>;
    using XYu = X_Y<uint32_t>;
    using XYf = X_Y<float>;
    using XYd = X_Y<double>;
    using XY = XYf;


    template<typename T>
    struct IsX_Y : std::false_type {};
    template<typename T>
    struct IsX_Y<X_Y<T>> : std::true_type {};
    template<typename T>
    struct IsX_Y<X_Y<T>&> : std::true_type {};
    template<typename T>
    struct IsX_Y<X_Y<T> const&> : std::true_type {};
    template<typename T>
    constexpr bool IsX_Y_v = IsX_Y<T>::value;


	template<typename T>
	struct DataFuncs<T, std::enable_if_t<IsX_Y_v<T>>> {
		template<bool needReserve = true>
		static inline void Write(Data& d, T const& in) {
			d.Write<needReserve>(in.x, in.y);
		}
		static inline int Read(Data_r& d, T& out) {
			return d.Read(out.x, out.y);
		}
	};

	template<typename T>
	struct StringFuncs<X_Y<T>, void> {
		static inline void Append(std::string& s, X_Y<T> const& in) {
			::xx::Append(s, in.x, ", ", in.y);
		}
	};

}
