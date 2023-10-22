#include <pch.h>
#include <tp.h>

int main() {
	auto&& cp = std::filesystem::current_path();
	std::cout << "tool: *.plist -> *.blist\nworking dir: " << cp.string() << "\npress any key continue...\n";
	std::cin.get();

	int n = 0;
	for (auto&& entry : std::filesystem::/*recursive_*/directory_iterator(cp)) {
		if (!entry.is_regular_file()) continue;
		auto&& p = entry.path();
		if (p.extension() != u8".plist") continue;
		auto fullpath = std::filesystem::absolute(p).u8string();
		auto newPath = fullpath.substr(0, fullpath.size() - 6) + u8".blist";
		auto newPath2 = fullpath.substr(0, fullpath.size() - 6) + u8".txt";

		TP tp;
		tp.Load((std::string&)p.u8string(), true);
		std::cout << "handle file: " << p << std::endl;
		xx::Data d;
		d.Write(tp.premultiplyAlpha);
		d.Write(tp.realTextureFileName);
		d.Write(tp.frames.size());
		for (auto& f : tp.frames) {
			std::cout << "handle frame: " << f->key << std::endl;
			d.Write(f->key);
			d.Write(f->anchor);
			d.Write((xx::RWFloatUInt16&)f->spriteOffset.x, (xx::RWFloatUInt16&)f->spriteOffset.y);
			d.Write((xx::RWFloatUInt16&)f->spriteSize.x, (xx::RWFloatUInt16&)f->spriteSize.y);
			d.Write((xx::RWFloatUInt16&)f->spriteSourceSize.x, (xx::RWFloatUInt16&)f->spriteSourceSize.y);
			d.WriteFixed(f->textureRect.x);
			d.WriteFixed(f->textureRect.y);
			d.WriteFixed(f->textureRect.w);
			d.WriteFixed(f->textureRect.h);
		}
		xx::WriteAllBytes(newPath, d);
		++n;

#if 0
		// verify code
		bool premultiplyAlpha;
		std::string realTextureFileName;
		size_t numFrames{};
		if (int r = d.Read(premultiplyAlpha, realTextureFileName, numFrames)) return r;
		for (size_t i = 0; i < numFrames; ++i) {
			std::string frameKey;
			std::optional<XY> anchor;
			XY spriteOffset, spriteSize, spriteSourceSize;
			uint16_t x, y, w, h;
			if (int r = d.Read(frameKey, anchor)) return r;
			if (int r = d.Read((xx::RWFloatUInt16&)spriteOffset.x, (xx::RWFloatUInt16&)spriteOffset.y)) return r;
			if (int r = d.Read((xx::RWFloatUInt16&)spriteSize.x, (xx::RWFloatUInt16&)spriteSize.y)) return r;
			if (int r = d.Read((xx::RWFloatUInt16&)spriteSourceSize.x, (xx::RWFloatUInt16&)spriteSourceSize.y)) return r;
			if (int r = d.ReadFixed(x)) return r;
			if (int r = d.ReadFixed(y)) return r;
			if (int r = d.ReadFixed(w)) return r;
			if (int r = d.ReadFixed(h)) return r;
		}
#endif

		// generate c++ use code
		// frame name style: prefix _number [ _number ]
		// group by prefix_ [ _number ]
		std::set<std::string_view> keys;
		for (auto& f : tp.frames) {
			std::string_view k{ f->key };
			auto ks = k.size();
			size_t i{};
		LabRetry:
			if (i = k.find_first_of('_', i); i != k.npos) {
				++i;
				auto bak = i;
				for (; i < ks; ++i) {
					if (k[i] < '0' || k[i] > '9') goto LabRetry;
				}
				if (i == ks) {
					keys.insert(k.substr(0, bak));
				}
			} else {
				keys.insert(k);
			}
		}

		size_t maxLen{};
		std::string s;
		s.append("xx::List<xx::Ref<Frame>, int32_t>");
		for (auto& k : keys) {
			auto k2 = k.back() == '_' ? k.substr(0, k.size() - 1) : k;
			xx::Append(s, "\n    frames_", k2, ",");
			if (k2.size() > maxLen) {
				maxLen = k2.size();
			}
		}
		s.resize(s.size() - 1);
		s.append(";\n");
		for (auto& k : keys) {
			auto k2 = k.back() == '_' ? k.substr(0, k.size() - 1) : k;
			xx::Append(s, "\n        tp->GetToByPrefix(frames_", k2, xx::CharRepeater{ ' ', maxLen - k2.size()}, ", \"", k, "\"", xx::CharRepeater{ ' ', maxLen - k2.size() },");");
		}
		xx::WriteAllBytes(newPath2, s.data(), s.size());
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
