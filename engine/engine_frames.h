#pragma once
#include <engine_frame.h>

struct Frames {
    std::vector<xx::Ref<Frame>> frames;

    // get frame by key
    xx::Ref<Frame> const& Get(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        xx::CoutN(key, " is not found");
        xx_assert(false);
        return *(xx::Ref<Frame>*)0;
    }

    xx::Ref<Frame> const& Get(char const* buf, size_t len) const {
        return Get(std::string_view(buf, len));
    }

    // get frames by key's prefix + number. example: "p1 p2 p3" prefix is 'p'

    std::vector<xx::Ref<Frame>> GetByPrefix(std::string_view const& prefix) const {
        std::vector<xx::Ref<Frame>> fs;
        GetToByPrefix(fs, prefix);
        return fs;
    }

    void GetTo(std::vector<xx::Ref<Frame>>& fs, std::initializer_list<std::string_view> keys) const {
        for (auto& k : keys) {
            fs.push_back(Get(k));
        }
    }

    size_t GetToByPrefix(std::vector<xx::Ref<Frame>>& fs, std::string_view const& prefix) const {
        size_t n{};
        for (auto& f : frames) {
            if (f->key.starts_with(prefix)) {
                auto s = f->key.substr(prefix.size());
                if (s[0] >= '0' && s[0] <= '9') {
                    fs.push_back(f);
                    ++n;
                }
            }
        }
        return n;
    }

    template<typename List>
    void GetTo(List& fs, std::initializer_list<std::string_view> keys) const {
        for (auto& k : keys) {
            fs.Emplace(Get(k));
        }
    }

    template<typename List>
    size_t GetToByPrefix(List& fs, std::string_view const& prefix) const {
        size_t n{};
        for (auto& f : frames) {
            if (f->key.starts_with(prefix)) {
                auto s = f->key.substr(prefix.size());
                if (s[0] >= '0' && s[0] <= '9') {
                    fs.Emplace(f);
                    ++n;
                }
            }
        }
        return n;
    }

    xx::Ref<Frame> TryGet(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        return {};
    }

    std::unordered_map<std::string_view, xx::Ref<Frame>> GetMapSV() const {
        std::unordered_map<std::string_view, xx::Ref<Frame>> fs;
        for (auto& f : frames) {
            fs[std::string_view(f->key)] = f;
        }
        return fs;
    }

    std::unordered_map<std::string, xx::Ref<Frame>> GetMapS() const {
        std::unordered_map<std::string, xx::Ref<Frame>> fs;
        for (auto& f : frames) {
            fs[f->key] = f;
        }
        return fs;
    }
};
