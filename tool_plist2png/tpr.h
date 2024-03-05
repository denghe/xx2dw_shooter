#pragma once
#include <xx_data.h>
#include <pugixml.hpp>

namespace TexturePackerReader {

	struct Anchor { float x{}, y{}; };
	struct Offset { float x{}, y{}; };
	struct Size { uint16_t width{}, height{}; };
	struct Rect { uint16_t x{}, y{}, width{}, height{}; };

	struct Plist {
		std::string version;

		struct Frame {
			std::string name;
			std::vector<std::string> aliases;
			std::optional<Anchor> anchor;			// new version
			Rect spriteColorRect;					// old
			Offset spriteOffset;
			Size spriteSize;
			Size spriteSourceSize;
			std::optional<bool> spriteTrimmed{};	// old
			Rect textureRect;
			bool textureRotated{};
			std::vector<uint16_t> triangles;		// new version
			std::vector<uint16_t> vertices;			// new version
			std::vector<uint16_t> verticesUV;		// new version
		};
		std::vector<Frame> frames;

		struct Metadata {
			std::string version;					// old
			int format{};
			bool premultiplyAlpha{};				// new version
			std::string realTextureFileName;		// new version
			Size size;
			std::string name;						// old
			std::string smartupdate;				// new version
			std::string textureFileName;
			bool premultipliedAlpha{};				// old
		}
		metadata;

		int Load(xx::Data const& fileData) {
			pugi::xml_document doc;
			if (auto&& r = doc.load_buffer(fileData.buf, fileData.len); r.status) {
				std::cerr << "load_buffer error : " << r.description() << std::endl;
				return -1;
			}

			auto xml_plist = doc.first_child();
			assert(xml_plist.name() == "plist"sv);
			version = xml_plist.attribute("version").value();
			assert(version == "1.0"sv);

			auto xml_dict = xml_plist.first_child();
			assert(xml_dict.name() == "dict"sv);

			auto k = xml_dict.first_child();
			CheckKey(k, "frames"sv);

			auto xml_frames = k.next_sibling();
			assert(xml_frames.name() == "dict"sv);

			pugi::xml_node v;
			std::string kn;

			auto xml_frames_children = xml_frames.children();
			for (auto iter = xml_frames_children.begin(); iter != xml_frames_children.end(); ++iter) {
				auto& frame = frames.emplace_back();
				assert(iter->name() == "key"sv);
				frame.name = iter->first_child().text().as_string();
				++iter;
				assert(iter->name() == "dict"sv);

				kn = GetKey(k = iter->first_child());
				if (kn == "frame"sv) {
					// format: 2
					frame.textureRect = ReadRect(v = k.next_sibling());

					CheckKey(k = v.next_sibling(), "offset"sv);
					frame.spriteOffset = ReadOffset(v = k.next_sibling());

					CheckKey(k = v.next_sibling(), "rotated"sv);
					frame.textureRotated = ReadBoolean(v = k.next_sibling());

					CheckKey(k = v.next_sibling(), "sourceSize"sv);
					frame.spriteSourceSize = ReadSize(v = k.next_sibling());
					frame.spriteSize = frame.spriteSourceSize;

				} else {
					// format: 3
					assert(kn == "aliases"sv);

					v = k.next_sibling();
					assert(v.name() == "array"sv);
					auto vc = v.children();
					for (auto it = vc.begin(); it != vc.end(); ++it) {
						frame.aliases.emplace_back(ReadString(*it));
					}

					kn = GetKey(k = v.next_sibling());
					if (kn == "anchor"sv) {
						frame.anchor = ReadAnchor(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "spriteColorRect"sv) {
						frame.spriteColorRect = ReadRect(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "spriteOffset"sv) {
						frame.spriteOffset = ReadOffset(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "spriteSize"sv) {
						frame.spriteSize = ReadSize(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "spriteSourceSize"sv) {
						frame.spriteSourceSize = ReadSize(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "spriteTrimmed"sv) {
						frame.spriteTrimmed = ReadBoolean(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "textureRect"sv) {
						frame.textureRect = ReadRect(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "textureRotated"sv) {
						frame.textureRotated = ReadBoolean(v = k.next_sibling());
						kn = GetKey(k = v.next_sibling());
					}
					if (kn == "triangles"sv) {
						frame.triangles = ReadIntList(v = k.next_sibling());

						CheckKey(k = v.next_sibling(), "vertices"sv);
						frame.vertices = ReadIntList(v = k.next_sibling());

						CheckKey(k = v.next_sibling(), "verticesUV"sv);
						frame.verticesUV = ReadIntList(v = k.next_sibling());
					}
				}
			}

			k = xml_frames.next_sibling();
			CheckKey(k, "metadata"sv);

			v = k.next_sibling();
			assert(v.name() == "dict"sv);

			kn = GetKey(k = v.first_child());
			if (kn == "version"sv) {
				metadata.version = ReadString(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "format"sv) {
				metadata.format = ReadInteger(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (metadata.format == 2 && kn == "textureFileName"sv) {
				metadata.textureFileName = ReadString(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "premultiplyAlpha"sv) {
				metadata.premultiplyAlpha = ReadBoolean(v = k.next_sibling());
				metadata.premultipliedAlpha = metadata.premultiplyAlpha;
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "realTextureFileName"sv) {
				metadata.realTextureFileName = ReadString(v = k.next_sibling());
				metadata.name = metadata.realTextureFileName;
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "size"sv) {
				metadata.size = ReadSize(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "name"sv) {
				metadata.name = ReadString(v = k.next_sibling());
				metadata.realTextureFileName = metadata.name;
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "smartupdate"sv) {
				metadata.smartupdate = ReadString(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "textureFileName"sv) {
				metadata.textureFileName = ReadString(v = k.next_sibling());
				kn = GetKey(k = v.next_sibling());
			}
			if (kn == "premultipliedAlpha"sv) {
				metadata.premultipliedAlpha = ReadBoolean(v = k.next_sibling());
				metadata.premultiplyAlpha = metadata.premultipliedAlpha;
			}

			return 0;
		}

		// helpers

		std::string GetKey(pugi::xml_node& n) {
			if (!n) return {};
			assert(n.name() == "key"sv);
			return n.first_child().text().as_string();
		}

		void CheckKey(pugi::xml_node& n, std::string_view key) {
			assert(n.name() == "key"sv);
			assert(n.first_child().text().as_string() == key);
		}

		// helpers

		std::vector<uint16_t> ReadIntList(pugi::xml_node& n) {
			auto s = ReadString(n);
			std::vector<uint16_t> r;
			ReadInts(r, s);
			return r;
		}

		std::string ReadString(pugi::xml_node& n) {
			assert(n.name() == "string"sv);
			return n.first_child().text().as_string();
		}

		int ReadInteger(pugi::xml_node& n) {
			assert(n.name() == "integer"sv);
			return n.first_child().text().as_int();
		}

		Anchor ReadAnchor(pugi::xml_node& n) {
			auto s = ReadString(n);
			Anchor a;
			ReadNum2(s, a.x, a.y);
			return a;
		}

		Offset ReadOffset(pugi::xml_node& n) {
			auto s = ReadString(n);
			Offset a;
			ReadNum2(s, a.x, a.y);
			return a;
		}

		Size ReadSize(pugi::xml_node& n) {
			auto s = ReadString(n);
			Size r;
			ReadNum2(s, r.width, r.height);
			return r;
		}

		Rect ReadRect(pugi::xml_node& n) {
			auto s = ReadString(n);
			Rect r;
			ReadInt4(s, r.x, r.y, r.width, r.height);
			return r;
		}

		bool ReadBoolean(pugi::xml_node& n) {
			return n.name() == "true"sv;
		}


		// helpers

		// format: 123
		static int ReadInt(const char* p) {
			int x = 0;
			while (*p >= '0') {
				if (*p > '9') break;
				x = (x * 10) + (*p - '0');
				++p;
			}
			return x;
		}

		// format: {1.2, 3.4}
		// T only support float, uint
		template<typename T>
		static void ReadNum2(std::string_view s, T& a, T& b) {
			auto n1Pos = s.find('{', 0) + 1;
			auto dotPos = s.find(',', 1);
			auto n1Len = dotPos - n1Pos;
			auto n2Pos = dotPos + 1;
			if (s[n2Pos] == ' ') ++n2Pos;
			auto n2Len = s.find('}', n2Pos) - n2Pos;

			auto as = s.substr(n1Pos, n1Len);
			auto bs = s.substr(n2Pos, n2Len);
			if constexpr (std::is_same_v<T, float>) {
				a = (T)atof(std::string(as).c_str());
				b = (T)atof(std::string(bs).c_str());
			} else {
				a = ReadInt(as.data());
				b = ReadInt(bs.data());
			}
		}

		// format: {{2, 9}, {28, 28}}
		template<typename T>
		static void ReadInt4(std::string_view s, T& a, T& b, T& c, T& d) {
			auto n1Pos = s.find('{', 0) + 2;
			auto dotPos = s.find(',', n1Pos);
			auto n2Pos = dotPos + 1;
			if (s[n2Pos] == ' ') ++n2Pos;
			auto n2PosEnd = s.find('}', n2Pos);

			auto n3Pos = s.find('{', n2PosEnd) + 1;
			dotPos = s.find(',', n3Pos);
			auto n4Pos = dotPos + 1;
			if (s[n4Pos] == ' ') ++n4Pos;

			a = ReadInt(s.data() + n1Pos);
			b = ReadInt(s.data() + n2Pos);
			c = ReadInt(s.data() + n3Pos);
			d = ReadInt(s.data() + n4Pos);
		}

		// ' ' continue，'<' quit. ascii： ' ' < '0-9' < '<'
		template<typename T>
		static void ReadInts(std::vector<T>& vs, std::string_view s) {
			vs.clear();
			auto p = s.data();
			auto e = p + s.size();
			do {
				int x = 0;
			LabLoop:
				if (*p >= '0' && *p <= '9') {
					x = (x * 10) + (*p - '0');
					++p;
					goto LabLoop;
				}
				vs.push_back(x);
				++p;
			} while (p < e);
		}

	};

}
