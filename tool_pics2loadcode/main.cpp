#include <pch.h>

int main() {
	SetConsoleOutputCP(CP_UTF8);
	auto&& cp = std::filesystem::current_path();
	std::cout << "tool: *.png -> code.txt\nworking dir: " << cp.string() << "\npress any key continue...\n";
	std::cin.get();

	std::vector<std::string> fns;	// not include ".png"
	for (auto&& entry : std::filesystem::/*recursive_*/directory_iterator(cp)) {
		if (!entry.is_regular_file()) continue;
		auto&& p = entry.path();
		if (p.extension() != u8".png"sv) continue;
		auto&& pn = p.filename().string();
		std::string_view sv(pn.c_str(), pn.size() - 4);	// remove ".png"
		fns.emplace_back(sv);
	}

	std::string h, c;

	xx::Append(h, R"#(.h:

)#");

	xx::Append(c, R"#(

.cpp:

// begin load / download textures
std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
std::string picRoot("put root / path here /");
)#");

	for (auto& fn : fns) {

		xx::AppendFormat(h, R"#(
xx::Ref<Frame> frame_{0};)#", fn);

		xx::AppendFormat(c, R"#(
ffs.emplace_back(picRoot + "{0}.png", &frame_{0});)#", fn);
	}

	xx::Append(c, R"#(

// load / download
#ifdef __EMSCRIPTEN__
	int32_t downloadCount{};
#endif
	for (auto& ff : ffs) {
#ifdef __EMSCRIPTEN__
	tasks.Add([pff = &ff, &downloadCount, this]()->xx::Task<> {
		auto& ff = *pff;
		*ff.second = co_await AsyncLoadFrameFromUrl(ff.first);
		++downloadCount;
	});
#else
	*ff.second = LoadFrame(ff.first);
#endif
}
#ifdef __EMSCRIPTEN__
	while (downloadCount < ffs.size()) co_yield 0;
#endif

// batch combine textures
auto ok = DynamicTexturePacker<512>::Pack(ffs);
assert(ok);

)#");


	// file name style: prefix _number [ _number ]
	// group by prefix_ [ _number ]
	std::map<std::string_view, std::vector<std::string_view>> kvs;
	for (auto& fn : fns) {
		std::string_view sv(fn);
		if (auto idx = sv.find_last_of('_'); idx != sv.npos) {
			auto k = sv.substr(0, idx);
			auto v = sv.substr(idx + 1);
			if (v.find_first_not_of("0123456789"sv) != v.npos) continue;
			kvs[k].push_back(v);
		}
	}

	// gen group name frames_xxxx
	for (auto&& kv : kvs) {
		xx::AppendFormat(h, R"(
xx::Listi32<xx::Ref<Frame>> frames_{0};)", kv.first);

		for (auto&& s : kv.second) {
			xx::AppendFormat(c, R"(
frames_{0}.Add(frame_{0}_{1});)", kv.first, s);
		}
	}

	// combine
	h.append(c);

	// save to file
	if (int r = xx::WriteAllBytes("code.txt", h.data(), h.size())) {
		std::cerr << "write file code.txt failed! r = " << r << std::endl;
		return -1;
	}

	xx::CoutFormat(R"(finished! handled {0} files! generated code.txt! press any key continue...)", fns.size());
	std::cin.get();

	return 0;
}
