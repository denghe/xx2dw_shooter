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

	// group by prefix...._number
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

	std::string h, c;

	xx::Append(h, R"#(#include <engine.h>

struct ResFrames {
	xx::Task<> AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz = 2048);
	xx::Task<> AsyncLoad(std::string picRoot, int32_t texSiz = 2048);

)#");

	for (auto& fn : fns) {
		xx::AppendFormat(h, R"#(
	xx::Ref<Frame> {0};)#", fn);
	}

	if (!kvs.empty()) {
		xx::Append(h, R"#(
)#");
	}

	for (auto&& kv : kvs) {
		xx::AppendFormat(h, R"(
	xx::Listi32<xx::Ref<Frame>> {0}_;)", kv.first);
	}

	xx::Append(h, R"#(
};
)#");


	xx::Append(c, R"#(#include "pch.h"
#include "res_frames.h"

xx::Task<> ResFrames::AsyncLoad(std::string picRoot, int32_t texSiz) {
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	co_return co_await AsyncLoad(ffs, picRoot, texSiz);
}

xx::Task<> ResFrames::AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz) {)#");

	for (auto& fn : fns) {
		xx::AppendFormat(c, R"#(
	ffs.emplace_back(picRoot + "{0}.png", &{0});)#", fn);
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
			*ff.second = co_await EngineBase3::Instance().AsyncLoadFrameFromUrl(ff.first);
			++downloadCount;
		});
#else
		*ff.second = EngineBase3::Instance().LoadFrame(ff.first);
#endif
	}
#ifdef __EMSCRIPTEN__
	while (downloadCount < ffs.size()) co_yield 0;
#endif

	// batch combine textures
	if (texSiz) {
		auto ok = DynamicTexturePacker<>::Pack(ffs, texSiz);
		assert(ok);
	}
)#");

	if (!kvs.empty()) {
		xx::Append(c, R"#(
	// fill groups
)#");
	}

	for (auto&& kv : kvs) {
		for (auto&& s : kv.second) {
			xx::AppendFormat(c, R"(
	{0}_.Add({0}_{1});)", kv.first, s);
		}
	}

	xx::Append(c, R"#(
	co_return;
}
)#");

	// save to file
	if (int r = xx::WriteAllBytes("res_frames.h", h.data(), h.size())) {
		std::cerr << "write file res_frames.h failed! r = " << r << std::endl;
		return -1;
	}
	if (int r = xx::WriteAllBytes("res_frames.cpp", c.data(), c.size())) {
		std::cerr << "write file res_frames.h failed! r = " << r << std::endl;
		return -1;
	}

	xx::CoutFormat(R"(finished! handled {0} files! generated res_frames.h & cpp! press any key continue...)", fns.size());
	std::cin.get();

	return 0;
}
