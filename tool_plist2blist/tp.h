#pragma once
#include <pch.h>

// texture packer's data container. fill data from export for cocos 3.x .plist file
struct TP : TexturePacker {

	void Load(std::string_view plistFn, bool sortByName = true) {
		std::string rootPath;
		if (auto&& [d, fp] = gEngine->LoadFileData(plistFn); !d) {
			throw std::logic_error("read file error: " + std::string(plistFn));
		} else {
			if (auto&& i = fp.find_last_of("/"); i != fp.npos) {
				rootPath = fp.substr(0, i + 1);
			}
			if (int r = Load(d, rootPath)) {
				throw std::logic_error(xx::ToString("parse plist file content error: r = ", r, ", fn = ", fp));
			}
		}

		if (sortByName) {
			std::sort(frames.begin(), frames.end(), [](xx::Ref<Frame> const& a, xx::Ref<Frame> const& b) {
				return xx::InnerNumberToFixed(a->key) < xx::InnerNumberToFixed(b->key);
			});
		}

	}

	int Load(std::string_view text, std::string_view const& rootPath) {
		frames.clear();
		size_t i, j;

		if (i = text.find(">frames<"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 19);													// skip    >frames</key><dict>

		if (i = text.find("<key>"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

	LabBegin:
		auto&& o = *frames.emplace_back().Emplace();
		if (j = text.find('<'); j == text.npos) return __LINE__;				// </key>
		o.key = CutStr(text, 0, j);
		CutStr(text, j + 30);													// skip    </key><dict><key>aliases</key>

		if (i = text.find("/>"sv); i == text.npos) return __LINE__;				// <array/>
		CutStr(text, i + 2);													// skip    <array/>

		if (i = text.find('<'); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

		if (text.starts_with('a')) {											// anchor
			CutStr(text, 20);													// skip    anchor</key><string>

			if (i = text.find('{'); i == text.npos) return __LINE__;
			if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
			o.anchor.emplace();
			if (ReadFloat2(CutStr(text, i + 1, j - i - 1), o.anchor->x, o.anchor->y)) return __LINE__;
			CutStr(text, j + 10);												// skip    }</string>
		}

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadFloat2(CutStr(text, i + 1, j - i - 1), o.spriteOffset.x, o.spriteOffset.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSize.x, o.spriteSize.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find('{'); i == text.npos) return __LINE__;
		if (j = text.find('}', i + 4); j == text.npos) return __LINE__;
		if (ReadInteger2(CutStr(text, i + 1, j - i - 1), o.spriteSourceSize.x, o.spriteSourceSize.y)) return __LINE__;
		CutStr(text, j + 10);													// skip    }</string>

		if (i = text.find("{{"sv); i == text.npos) return __LINE__;
		if (j = text.find("}}"sv, i + 10); j == text.npos) return __LINE__;
		ReadInteger4(CutStr(text, i + 2, j - i - 2), o.textureRect.x, o.textureRect.y, o.textureRect.w, o.textureRect.h);
		CutStr(text, j + 11);													// skip    }}</string>

		if (i = text.find("</k"sv); i == text.npos) return __LINE__;			// <key>textureRotated</key>
		CutStr(text, i + 6);													// skip    </key>

		if (i = text.find('<'); i == text.npos) return __LINE__;				// locate <true || <false
		CutStr(text, i + 1);													// skip    <

		if (text.starts_with('t')) {
			o.textureRotated = true;
			CutStr(text, 6);													// skip    true/>
		} else {
			o.textureRotated = false;
			CutStr(text, 7);													// skip    false/>
		}

		if (i = text.find("<k"sv); i == text.npos) return __LINE__;
		CutStr(text, i + 5);													// skip    <key>

		if (text.starts_with("triangles<"sv)) {
			CutStr(text, 15);													// skip    triangles</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.triangles, CutStr(text, i + 8, j - i - 8));

			CutStr(text, j + 9);												// skip    </string>
			if (i = text.find('<'); i == text.npos) return __LINE__;
			CutStr(text, i + 19);												// skip    <key>vertices</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.vertices, CutStr(text, i + 8, j - i - 8));

			CutStr(text, j + 9);												// skip    </string>
			if (i = text.find('<'); i == text.npos) return __LINE__;
			CutStr(text, i + 21);												// skip    <key>verticesUV</key>

			if (i = text.find('<'); i == text.npos) return __LINE__;			// <string>
			if (j = text.find('<', i + 8); j == text.npos) return __LINE__;		// </string>
			SpaceSplitFillVector(o.verticesUV, CutStr(text, i + 8, j - i - 8));

			{
				auto len = o.vertices.size() / 2;								// flip y for ogl
				auto p = (XY*)o.vertices.data();
				for (size_t k = 0; k < len; ++k) {
					p[k].y = o.spriteSize.y - p[k].y;
				}
			}

			CutStr(text, j + 9);												// skip    </string>

			if (i = text.find("<k"sv); i == text.npos) return __LINE__;
			CutStr(text, i + 5);												// skip    <key>
		}

		if (!text.starts_with("metadata<"sv)) {									// <key>metadata</key>    ||    <key> frame name </key>
			goto LabBegin;
		}

		CutStr(text, 20);														// skip    metadata</key><dict>

		if (i = text.find(">pre"sv); i == text.npos) return __LINE__;			// <key>premultiplyAlpha</key>
		CutStr(text, i + 23);													// skip    >premultiplyAlpha</key>

		if (i = text.find('<'); i == text.npos) return __LINE__;				// locate <true || <false
		CutStr(text, i + 1);													// skip    <

		if (text.starts_with('t')) {
			premultiplyAlpha = true;
			CutStr(text, 6);													// skip    true/>
		} else {
			premultiplyAlpha = false;
			CutStr(text, 7);													// skip    false/>
		}

		// realTextureFileName
		if (i = text.find("<s"sv); i == text.npos) return __LINE__;				// <string> tex file name .ext
		if (j = text.find('<', i + 8); j == text.npos) return __LINE__;			// </string>
		realTextureFileName = rootPath;
		realTextureFileName.append(CutStr(text, i + 8, j - i - 8));

		return 0;
	}


	// format: a,b
	template<typename T>
	static int ReadFloat2(std::string_view const& line, T& a, T& b) {
		auto dotPos = line.find(',', 1);
		if (dotPos == std::string_view::npos) return __LINE__;
#ifdef _MSVC
		std::from_chars(line.data(), line.data() + dotPos, a);
		std::from_chars(line.data() + dotPos + 1, line.data() + line.size(), b);
#else
        a = (T)atof(std::string(line.substr(0, dotPos)).c_str());
        b = (T)atof(std::string(line.substr(dotPos + 1)).c_str());
#endif
		return 0;
	}

	template<bool check9 = false>
	static int ReadInteger(const char*& p) {
		int x = 0;
		while (*p >= '0') {
			if constexpr (check9) {
				if (*p > '9') break;
			}
			x = (x * 10) + (*p - '0');
			++p;
		}
		return x;
	}

	// format: a,b
	// ascii： ',' < '0~9' < '>}'
	template<typename T>
	static int ReadInteger2(std::string_view const& line, T& a, T& b) {
		auto&& p = line.data();
		a = (T)ReadInteger(p);
		++p;	// skip ','
		b = (T)ReadInteger<true>(p);
		return 0;
	}

	// format: a,b},{c,d
	template<typename T>
	static void ReadInteger4(std::string_view const& line, T& a, T& b, T& c, T& d) {
		auto&& p = line.data();
		a = (T)ReadInteger(p);
		++p;	// skip ','
		b = (T)ReadInteger<true>(p);
		p += 3;	// skip '},{'
		c = (T)ReadInteger(p);
		++p;	// skip ','
		d = (T)ReadInteger<true>(p);
	}

	// ' ' continue，'<' quit. ascii： ' ' < '0-9' < '<'
	template<typename T>
	static void SpaceSplitFillVector(std::vector<T>& vs, std::string_view const& line) {
		vs.clear();
		auto&& p = line.data();
	LabBegin:
		vs.push_back((T)ReadInteger<true>(p));
		if (*p == ' ') {
			++p;
			goto LabBegin;
		}
	}

	// unsafe for speed up
	XX_INLINE static void CutStr(std::string_view& sv, size_t const& i) {
		sv = { sv.data() + i, sv.size() - i };
	}
	XX_INLINE static std::string_view CutStr(std::string_view const& sv, size_t const& i, size_t const& siz) {
		return { sv.data() + i, siz };
	}
};
