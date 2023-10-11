#pragma once
#include <engine_frame.h>

struct Frames {
    std::vector<xx::Shared<Frame>> frames;

    // get frame by key
    xx::Shared<Frame> const& Get(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        throw std::logic_error(xx::ToString(key, " is not found"));
    }

    xx::Shared<Frame> const& Get(char const* const& buf, size_t const& len) const {
        return Get(std::string_view(buf, len));
    }

    // get frames by key's prefix + number. example: "p1 p2 p3" prefix is 'p'

    std::vector<xx::Shared<Frame>> GetByPrefix(std::string_view const& prefix) const {
        std::vector<xx::Shared<Frame>> fs;
        GetToByPrefix(fs, prefix);
        return fs;
    }

    void GetTo(std::vector<xx::Shared<Frame>>& fs, std::initializer_list<std::string_view> keys) const {
        for (auto& k : keys) {
            fs.push_back(Get(k));
        }
    }

    size_t GetToByPrefix(std::vector<xx::Shared<Frame>>& fs, std::string_view const& prefix) const {
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

    xx::Shared<Frame> TryGet(std::string_view const& key) const {
        for (auto& f : frames) {
            if (f->key == key) return f;
        }
        return {};
    }

    std::unordered_map<std::string_view, xx::Shared<Frame>> GetMapSV() const {
        std::unordered_map<std::string_view, xx::Shared<Frame>> fs;
        for (auto& f : frames) {
            fs[std::string_view(f->key)] = f;
        }
        return fs;
    }

    std::unordered_map<std::string, xx::Shared<Frame>> GetMapS() const {
        std::unordered_map<std::string, xx::Shared<Frame>> fs;
        for (auto& f : frames) {
            fs[f->key] = f;
        }
        return fs;
    }
};
