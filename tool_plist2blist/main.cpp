#include <pch.h>

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

		xx::Data fd;
		if (int r = xx::ReadAllBytes(p, fd)) {
			std::cerr << "ReadAllBytes failed. r = " << r << " fn = " << p << std::endl;
			return -1;
		}

		TexturePackerReader::Plist tp;
		if (int r = tp.Load(fd)) {
			std::cerr << "tp.Load failed. r = " << r << " fn = " << p << std::endl;
			return -2;
		}

		std::cout << "handle file: " << p << std::endl;
		xx::Data d;
		d.WriteBuf("blist_1 ");	// custom file head 8 bytes
		d.Write(tp.metadata.realTextureFileName);
		d.Write(tp.metadata.premultiplyAlpha);
		d.Write(tp.frames.size());
		for (auto& f : tp.frames) {
			std::cout << "handle frame: " << f.name << std::endl;
			d.Write(f.name);
			d.Write(f.aliases);
			if (f.anchor.has_value()) {
				d.WriteFixed((uint8_t)1);
				d.Write(f.anchor->x, f.anchor->y);
			} else {
				d.WriteFixed((uint8_t)0);
			}
			d.WriteFixed(f.spriteOffset.x);
			d.WriteFixed(f.spriteOffset.y);
			d.WriteFixed(f.spriteSize.width);
			d.WriteFixed(f.spriteSize.height);
			d.WriteFixed(f.spriteSourceSize.width);
			d.WriteFixed(f.spriteSourceSize.height);
			d.WriteFixed(f.textureRect.x);
			d.WriteFixed(f.textureRect.y);
			d.WriteFixed(f.textureRect.width);
			d.WriteFixed(f.textureRect.height);
			d.WriteFixed(f.textureRotated);
		}
		xx::WriteAllBytes(newPath, d);
		++n;

		// generate c++ use code
		// frame name style: prefix _number [ _number ]
		// group by prefix_ [ _number ]
		std::set<std::string_view> keys;
		for (auto& f : tp.frames) {
			std::string_view k{ f.name };
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
			std::string kk( k.back() == '_' ? k.substr(0, k.size() - 1) : k );
			for (auto& c : kk) {
				if (c == '.') c = '_';
			}
			std::string_view k2(kk);

			xx::Append(s, "\n    frames_", k2, ",");
			if (k2.size() > maxLen) {
				maxLen = k2.size();
			}
		}
		s.resize(s.size() - 1);
		s.append(";\n");
		for (auto& k : keys) {
			std::string kk(k.back() == '_' ? k.substr(0, k.size() - 1) : k);
			for (auto& c : kk) {
				if (c == '.') c = '_';
			}
			std::string_view k2(kk);

			xx::Append(s, "\n        tp->GetToByPrefix(frames_", k2, xx::CharRepeater{ ' ', maxLen - k2.size() }, ", \"", k, "\"", xx::CharRepeater{ ' ', maxLen - k2.size() }, ");");
		}
		xx::WriteAllBytes(newPath2, s.data(), s.size());
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
