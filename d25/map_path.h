#pragma once
#include "track_manager.h"

enum class MoveTips : uint16_t {
	__begin, Left, Right, Up, Down, Begin, End, __end
};
constexpr std::array<std::string_view, (size_t)MoveTips::__end> MoveTips_txt{
	""sv, "left"sv, "right"sv,"up"sv,"down"sv,"begin"sv,"end"sv,
};

struct MapPath {
	xx::Listi32<xx::FromTo<MoveTips>> mapMoveTips;
	Vec2<> beginCRIdx{ -1,-1 }, endCRIdx{ -1,-1 };
	std::string_view name;
	TrackManager tm;

	void Init(xx::Ref<TMX::Map> const& map, TMX::Layer_Tile* layer, float tileSize) {
		name = layer->name;
		mapMoveTips.Resize((int)layer->gids.size());
		for (int i = 0, e = (int)layer->gids.size(); i < e; ++i) {
			if (auto gid = layer->gids[i]) {
				auto& gi = map->gidInfos[gid];
				auto& mt = (xx::FromTo<MoveTips>&)gi.ud;
				mapMoveTips[i] = mt;
				if (mt.from == MoveTips::Begin) {
					beginCRIdx.y = i / map->width;
					beginCRIdx.x = i - beginCRIdx.y * map->width;
				} else if (mt.to == MoveTips::End) {
					endCRIdx.y = i / map->width;
					endCRIdx.x = i - endCRIdx.y * map->width;
				}
				assert((uint16_t)mt.from > (uint16_t)MoveTips::__begin);
				assert((uint16_t)mt.to < (uint16_t)MoveTips::__end);
			}
		}

		// path to track ( fill tm )
		FillTM(map->width, tileSize);
	}

	// path to track ( fill tm )
	void FillTM(int32_t mapWidth, float tileSize) {
		assert(mapMoveTips.len > 1);
		assert(beginCRIdx.x >= 0 && beginCRIdx.y >= 0);
		assert(endCRIdx.x >= 0 && endCRIdx.y >= 0);

		// begin path to track ( fill tm )
		std::vector<CurvePoint> ps;

		int32_t rowIdx, colIdx;
		xx::FromTo<MoveTips> mt;
		XY pos;

		auto HandleFrom = [&] {
			mt = mapMoveTips[rowIdx * mapWidth + colIdx];
			switch (mt.from) {
			case MoveTips::Left:
				pos.x = colIdx * tileSize;
				pos.y = rowIdx * tileSize + tileSize / 2;
				break;
			case MoveTips::Right:
				pos.x = colIdx * tileSize + tileSize;
				pos.y = rowIdx * tileSize + tileSize / 2;
				break;
			case MoveTips::Up:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize;
				break;
			case MoveTips::Down:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize + tileSize;
				break;
			case MoveTips::Begin:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize + tileSize / 2;
				break;
			default:
				assert(false);
			}
			if (!ps.size() || ps.size() && ps.back().pos != pos) {
				ps.push_back({ pos });
			}
			};

		auto HandleTo = [&] {
			// add begin pos to & move to next
			switch (mt.to) {
			case MoveTips::Left:
				pos.x = colIdx * tileSize;
				pos.y = rowIdx * tileSize + tileSize / 2;
				--colIdx;
				break;
			case MoveTips::Right:
				pos.x = colIdx * tileSize + tileSize;
				pos.y = rowIdx * tileSize + tileSize / 2;
				++colIdx;
				break;
			case MoveTips::Up:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize;
				--rowIdx;
				break;
			case MoveTips::Down:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize + tileSize;
				++rowIdx;
				break;
			case MoveTips::End:
				pos.x = colIdx * tileSize + tileSize / 2;
				pos.y = rowIdx * tileSize + tileSize / 2;
				break;
			default:
				assert(false);
			}
			if (ps.back().pos != pos) {
				ps.push_back({ pos });
			}
			};

		// fill begin pos data
		rowIdx = beginCRIdx.y;
		colIdx = beginCRIdx.x;
		mt = mapMoveTips[rowIdx * mapWidth + colIdx];

		// handle all
		do {
			HandleFrom();
			HandleTo();
		} while (mt.to != MoveTips::End);

		tm.totalWidth = tileSize;
		tm.trackCount = 11;
		tm.pointDistance = 0.1f;
		tm.trackMargin = tm.totalWidth / tm.trackCount;
		tm.Init(ps);
	}

	// remove prefix & suffix, convert name to MoveTips and fill to gi.ud
	inline static void InitMapGidInfos(xx::Ref<TMX::Map> const& map) {
		static constexpr auto prefix = "td_path_"sv;
		static constexpr auto suffix = ".png"sv;
		for (auto& gi : map->gidInfos) {
			if (!gi) continue;
			if (gi.image->source.starts_with(prefix)) {
				auto& mt = (xx::FromTo<MoveTips>&)gi.ud;
				std::string_view sv(gi.image->source.c_str() + prefix.size(), gi.image->source.size() - prefix.size() - suffix.size());
				auto idx = sv.find('_');
				auto sv1 = sv.substr(0, idx);
				auto sv2 = sv.substr(idx + 1, sv.size() - idx - 1);

				auto it = std::find(MoveTips_txt.begin(), MoveTips_txt.end(), sv1);
				assert(it != MoveTips_txt.end());
				mt.from = (MoveTips)std::distance(MoveTips_txt.begin(), it);

				it = std::find(MoveTips_txt.begin(), MoveTips_txt.end(), sv2);
				assert(it != MoveTips_txt.end());
				mt.to = (MoveTips)std::distance(MoveTips_txt.begin(), it);

				//xx::CoutN(sv, " ", MoveTips_txt[(uint16_t)mt.from], " ", MoveTips_txt[(uint16_t)mt.to]);
			}
		}
	}
};
