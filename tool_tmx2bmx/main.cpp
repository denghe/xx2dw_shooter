#include "pch.h"
#include <engine_tiledmap_reader.h>

int main() {
	auto&& cp = std::filesystem::current_path();
	std::cout << "tool: *.tmx -> *.bmx\nworking dir: " << cp.string() << "\npress any key continue...\n";
	std::cin.get();
	int n = 0;
	for (auto&& entry : std::filesystem::/*recursive_*/directory_iterator(cp)) {
		if (!entry.is_regular_file()) continue;
		auto&& p = entry.path();
		if (p.extension() != u8".tmx") continue;
		auto fullpath = std::filesystem::absolute(p).u8string();
		auto newPath = fullpath.substr(0, fullpath.size() - 4) + u8".bmx";

		TMX::Map map;
		TMX::FillTo(map, (std::string&)p.u8string(), false);
		std::cout << "handle file: " << p << std::endl;
		xx::TmxData d;
		d.Write(map);
		xx::WriteAllBytes(newPath, d);

		TMX::Map map2;
		d.key = {};
		d.objs.clear();
		d.keys.clear();
		int r = d.Read(map2);
		xx_assert(!r);
		xx_assert(d.offset == d.len);
		// todo: more verify ?

		++n;
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
