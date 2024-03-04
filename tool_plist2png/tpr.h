#pragma once
#include <xx_data.h>
#include <pugixml.hpp>

namespace TexturePackerReader {

	struct Offset {
		float x{}, y{};
	};

	struct Size {
		float width{}, height{};
	};

	struct Rect : Offset, Size {};

	struct Plist {
		std::string version;

		struct Frame {
			std::string name;
			std::vector<std::string> aliases;
			Rect spriteColorRect;
			Offset spriteOffset;
			Size spriteSize;
			Size spriteSourceSize;
			bool spriteTrimmed{};
			Rect textureRect;
			bool textureRotated{};
		};
		std::vector<Frame> frames;

		struct Metadata {
			std::string version;
			int format{};
			Size size;
			std::string name;
			std::string textureFileName;
			bool premultipliedAlpha{};
		}
		metadata;

		void CheckKey(pugi::xml_node& n, std::string_view key) {
			assert(n.name() == "key"sv);
			assert(n.first_child().text().as_string() == key);
		}

		std::string ReadString(pugi::xml_node& n) {
			assert(n.name() == "string"sv);
			return n.first_child().text().as_string();
		}

		int ReadInteger(pugi::xml_node& n) {
			assert(n.name() == "integer"sv);
			return n.first_child().text().as_int();
		}

		Size ReadSize(pugi::xml_node& n) {
			assert(n.name() == "string"sv);
			std::string s = n.first_child().text().as_string();
			// todo: convert s to Size
			return {};
		}

		Offset ReadOffset(pugi::xml_node& n) {
			assert(n.name() == "string"sv);
			std::string s = n.first_child().text().as_string();
			// todo: convert s to Offset
			return {};
		}

		Rect ReadRect(pugi::xml_node& n) {
			assert(n.name() == "string"sv);
			std::string s = n.first_child().text().as_string();
			// todo: convert s to Rect
			return {};
		}

		bool ReadBoolean(pugi::xml_node& n) {
			return n.name() == "true"sv;
		}

		void Fill(std::string& tar, pugi::xml_node& n, std::string_view key) {
			auto xml_metadata_version_key = n.first_child();
			assert(xml_metadata_version_key.name() == "key"sv);
			assert(xml_metadata_version_key.first_child().text().as_string() == "version"sv);

			auto xml_metadata_version = xml_metadata_version_key.next_sibling();
			assert(xml_metadata_version.name() == "string"sv);
			metadata.version = xml_metadata_version.first_child().text().as_string();
		}

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

			auto xml_frames_children = xml_frames.children();
			for (auto iter = xml_frames_children.begin(); iter != xml_frames_children.end(); ++iter) {
				auto& frame = frames.emplace_back();
				assert(iter->name() == "key"sv);
				frame.name = iter->first_child().text().as_string();
				++iter;
				assert(iter->name() == "dict"sv);
				k = iter->first_child();
				CheckKey(k, "aliases"sv);
				v = k.next_sibling();
				assert(v.name() == "array"sv);
				auto vc = v.children();
				for (auto it = vc.begin(); it != vc.end(); ++it) {
					frame.aliases.emplace_back(ReadString(*it));
				}
				CheckKey(k = v.next_sibling(), "spriteColorRect"sv);
				frame.spriteColorRect = ReadRect(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "spriteOffset"sv);
				frame.spriteOffset = ReadOffset(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "spriteSize"sv);
				frame.spriteSize = ReadSize(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "spriteSourceSize"sv);
				frame.spriteSourceSize = ReadSize(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "spriteTrimmed"sv);
				frame.spriteTrimmed = ReadBoolean(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "textureRect"sv);
				frame.textureRect = ReadRect(v = k.next_sibling());

				CheckKey(k = v.next_sibling(), "textureRotated"sv);
				frame.textureRotated = ReadBoolean(v = k.next_sibling());
			}

			k = xml_frames.next_sibling();
			CheckKey(k, "metadata"sv);

			v = k.next_sibling();
			assert(v.name() == "dict"sv);

			CheckKey(k = v.first_child(), "version"sv);
			metadata.version = ReadString(v = k.next_sibling());

			CheckKey(k = v.next_sibling(), "format"sv);
			metadata.format = ReadInteger(v = k.next_sibling());

			CheckKey(k = v.next_sibling(), "size"sv);
			metadata.size = ReadSize(v = k.next_sibling());

			CheckKey(k = v.next_sibling(), "name"sv);
			metadata.name = ReadString(v = k.next_sibling());

			CheckKey(k = v.next_sibling(), "textureFileName"sv);
			metadata.textureFileName = ReadString(v = k.next_sibling());

			CheckKey(k = v.next_sibling(), "premultipliedAlpha"sv);
			metadata.premultipliedAlpha = ReadBoolean(v = k.next_sibling());

			return 0;
		}
	};

}
