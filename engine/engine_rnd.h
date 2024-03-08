#pragma once
#include <engine_includes.h>

// reference from https://github.com/Reputeless/Xoshiro-cpp
struct Rnd {
    std::array<uint32_t, 4> state;

    Rnd() {
        SetSeed(xx::NowEpoch10m());
    }
    Rnd(Rnd const&) = default;
    Rnd& operator=(Rnd const&) = default;

    void SetSeed(uint64_t seed) {
        auto calc = [&]()->uint64_t {
            auto z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            return z ^ (z >> 31);
            };
        auto v = calc();
        memcpy(&state[0], &v, 8);
        v = calc();
        memcpy(&state[2], &v, 8);
    }

    uint32_t Get() {
        auto rotl = [](uint32_t x, int s)->uint32_t {
            return (x << s) | (x >> (32 - s));
            };
        auto result = rotl(state[1] * 5, 7) * 9;
        auto t = state[1] << 9;
        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];
        state[2] ^= t;
        state[3] = rotl(state[3], 11);
        return result;
    }

    void NextBytes(void* buf, size_t len) {
        uint32_t v{};
        size_t i{};
        auto e = len & (std::numeric_limits<size_t>().max() - 3);
        for (; i < e; i += 4) {
            v = Get();
            memcpy((char*)buf + i, &v, 4);
        }
        if (i < len) {
            v = Get();
            memcpy((char*)buf + i, &v, len - i);
        }
    }

    std::string NextWord(size_t siz = 0, std::string_view chars = "abcdefghijklmnopqrstuvwxyz"sv) {
        assert(chars.size() < 256);
        if (!siz) {
            siz = Next(2, 10);
        }
        std::string s;
        s.resize(siz);
        NextBytes(s.data(), siz);
        for (auto& c : s) {
            c = chars[c % chars.size()];
        }
        return s;
    }

    template<typename V = int32_t, class = std::enable_if_t<std::is_arithmetic_v<V>>>
    V Next() {
        if constexpr (std::is_same_v<bool, std::decay_t<V>>) {
            return Get() >= std::numeric_limits<uint32_t>::max() / 2;
        } else if constexpr (std::is_integral_v<V>) {
            std::make_unsigned_t<V> v;
            if constexpr (sizeof(V) <= 4) {
                v = (V)Get();
            } else {
                v = (V)(Get() | ((uint64_t)Get() << 32));
            }
            if constexpr (std::is_signed_v<V>) {
                return (V)(v & std::numeric_limits<V>::max());
            } else return (V)v;
        } else if constexpr (std::is_floating_point_v<V>) {
            if constexpr (sizeof(V) == 4) {
                return (float)(double(Get()) / 0xFFFFFFFFu);
            } else if constexpr (sizeof(V) == 8) {
                constexpr auto max53 = (1ull << 53) - 1;
                auto v = ((uint64_t)Get() << 32) | Get();
                return double(v & max53) / max53;
            }
        }
        assert(false);
    }

    template<typename V>
    V Next(V from, V to) {
        if (from == to) return from;
        assert(from < to);
        if constexpr (std::is_floating_point_v<V>) {
            return from + Next<V>() * (to - from);
        } else {
            return from + Next<V>() % (to - from + 1);
        }
    }

    template<typename V>
    V Next2(V from, V to) {
        if (from == to) return from;
        if (to < from) {
            std::swap(from, to);
        }
        if constexpr (std::is_floating_point_v<V>) {
            return from + Next<V>() * (to - from);
        } else {
            return from + Next<V>() % (to - from + 1);
        }
    }

    template<typename V>
    V Next(V to) {
        return Next((V)0, to);
    }

    template<typename V>
    V Next(std::pair<V, V> const& fromTo) {
        return Next(fromTo.first, fromTo.second);
    }
};
