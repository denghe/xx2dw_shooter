#pragma once
#include "xx_blocklist.h"

namespace xx {
	template<typename T>
	struct SpaceNode : VersionNextIndexTypeId {
		int32_t prev, cidx;
		T value;
	};

	template<typename T>
	using SpaceWeak = BlockListWeak<T, SpaceNode>;

	struct SpaceCountRadius { int32_t count; float radius; };
	union SpaceIndex { 
		struct {
		int32_t x, y;
		};
		uint64_t data{};
	};
	struct SpaceXY { float x, y; };

	struct SpaceRingDiffuseData {
		xx::List<SpaceCountRadius, int32_t> lens;
		xx::List<SpaceIndex, int32_t> idxs;

		void Init(int32_t gridNumRows, int32_t gridCellDiameter) {
			auto step = (float)gridCellDiameter / 2;
			lens.Emplace(0, 0.f);
			SpaceIndex lastIdx{};
			idxs.Add(lastIdx);
			std::unordered_set<uint64_t> idxset;    // avoid duplicate
			for (float r = step; r < gridCellDiameter * gridNumRows; r += step) {
				auto c = 2 * M_PI * r;
				if (c < step) continue;
				auto lenBak = idxs.len;
				auto astep = float(M_PI * 2 * (step / c) / 10);
				auto rd = r / gridCellDiameter;
				for (float a = astep; a < M_PI * 2; a += astep) {
					SpaceIndex idx{ int32_t(rd * cos(a)), int32_t(rd * sin(a)) };
					if (lastIdx.data != idx.data && idxset.insert(idx.data).second) {
						idxs.Add(idx);
						lastIdx = idx;
					}
				}
				if (idxs.len > lenBak) {
					lens.Emplace(idxs.len, r);
				}
			}
		}
	};

	// requires
	// T has member: static constexpr int32_t cTypeId 
	// T has member: float x, y
	template<typename T>
	struct SpaceGrid : protected BlockList<T, SpaceNode> {
		using ST = BlockList<T, SpaceNode>;
		using ST::ST;
		using Node = SpaceNode<T>;

		using ST::Count;
		using ST::Foreach;

		int32_t numRows{}, numCols{}, cellSize{};
		int32_t maxX{}, maxY{};
		int32_t cellsLen{};
		std::unique_ptr<int32_t[]> cells;

		void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
			assert(!cells);
			assert(numRows_ > 0 && numCols_ > 0 && cellSize_ > 0);
			numRows = numRows_;
			numCols = numCols_;
			cellSize = cellSize_;
			maxX = cellSize_ * numCols_;
			maxY = cellSize_ * numRows_;

			cellsLen = numRows * numCols;
			cells = std::make_unique<int32_t[]>(cellsLen);
			memset(cells.get(), -1, sizeof(int32_t) * cellsLen);	// -1 mean empty
		}

		template<bool resetVersion = false>
		void Clear() {
			if (!cells) return;
			ST::template Clear<resetVersion>();
			memset(cells.get(), -1, sizeof(int32_t) * cellsLen);
		}

		// Emplace + Init( args ) + store inti cells[ pos ]
		template<typename U = T, typename...Args>
		U& EmplaceInit(Args&&...args) {
			assert(cells);
			auto index = ST::Alloc();
			auto& o = ST::RefNode(index);
			o.version = ST::GenVersion();
			o.next = -1;
			o.index = index;
			o.typeId = T::cTypeId;
			new (&o.value) U();
			o.value.Init(std::forward<Args>(args)...);

			auto cidx = PosToCIdx(o.value.x, o.value.y);
			auto head = cells[cidx];	// backup
			if (head >= 0) {
				ST::RefNode(head).prev = index;
			}
			cells[cidx] = index;			// assign new
			o.next = head;
			o.prev = -1;
			o.cidx = cidx;
			return (U&)o.value;
		}

		XX_FORCE_INLINE int32_t PosToCIdx(float x, float y) {
			assert(x >= 0 && x < cellSize * numCols);
			assert(y >= 0 && y < cellSize * numRows);
			auto c = int32_t(x) / cellSize;
			auto r = int32_t(y) / cellSize;
			return r * numCols + c;
		}

		void Remove(int32_t index) {
			auto& o = ST::RefNode(index);
			assert(o.version);
			assert(o.prev != o.index && o.next != o.index && o.version && o.cidx >= 0 && o.index == index);

			if (index == cells[o.cidx]) {
				cells[o.cidx] = o.next;
			}
			if (o.prev >= 0) {
				ST::RefNode(o.prev).next = o.next;
			}
			if (o.next >= 0) {
				ST::RefNode(o.next).prev = o.prev;
			}

			//o.typeId = -1;
			//o.index = -1;
			o.prev = -1;
			o.cidx = -1;
			o.version = 0;
			o.next = ST::freeHead;
			o.value.~T();
			ST::freeHead = index;
			++ST::freeCount;
			ST::FlagUnset(index);
		}

		void Remove(T const& v) {
			auto& o = *container_of(&v, Node, value);
			Remove(o.index);
		}

		void Remove(SpaceWeak<T> const& w) {
			if (!w.Exists()) return;
			Remove(w.RefNode().index);
		}

		void Update(T& v) {
			auto& o = *container_of(&v, Node, value);
			assert(o.index >= 0);
			assert(o.prev != o.index);
			assert(o.next != o.index);
			auto cidx = PosToCIdx(v.x, v.y);
			if (cidx == o.cidx) return;				// no change

			// unlink
			if (o.index != cells[o.cidx]) {			// isn't header
				ST::RefNode(o.prev).next = o.next;
				if (o.next >= 0) {
					ST::RefNode(o.next).prev = o.prev;
					//o.next = -1;
				}
				//o.prev = -1;
			} else {
				assert(o.prev == -1);
				cells[o.cidx] = o.next;
				if (o.next >= 0) {
					ST::RefNode(o.next).prev = -1;
					//o.next = -1;
				}
			}
			//o.cidx = -1;

			// relink
			if (cells[cidx] >= 0) {
				ST::RefNode(cells[cidx]).prev = o.index;
			}
			o.next = cells[cidx];
			o.prev = -1;
			cells[cidx] = o.index;
			o.cidx = cidx;
		}

		// .CellForeach([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
		template<typename F>
		void CellForeach(int32_t cidx, F&& func) {
			using R = xx::FuncR_t<F>;
			auto idx = cells[cidx];
			while (idx >= 0) {
				auto& o = ST::RefNode(idx);
				auto next = o.next;
				if constexpr (std::is_void_v<R>) {
					func(o.value);
				} else {
					auto r = func(o.value);
					if constexpr (std::is_same_v<R, bool>) {
						if (r) return;
					} else {
						switch (r) {
						case xx::ForeachResult::Continue: break;
						case xx::ForeachResult::Break: return;
						case xx::ForeachResult::RemoveAndContinue: {
							Remove(idx);
							break;
						}
						case xx::ForeachResult::RemoveAndBreak: {
							Remove(idx);
							return;
						}
						}
					}
				}
				idx = next;
			}
		}

		constexpr static std::array<SpaceIndex, 9> offsets9 = { SpaceIndex
			{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
		};

		// return x: col index   y: row index
		XX_FORCE_INLINE SpaceIndex PosToCrIdx(float x, float y) {
			assert(x >= 0 && x < cellSize * numCols);
			assert(y >= 0 && y < cellSize * numRows);
			return { int32_t(x) / cellSize, int32_t(y) / cellSize };
		}

		// return cell's index
		XX_FORCE_INLINE int32_t CrIdxToCIdx(int32_t colIdx, int32_t rowIdx) {
			return rowIdx * numCols + colIdx;
		}

		// foreach target cell + round 8 = 9 cells
		// .Foreach9([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
		template<typename F>
		void Foreach9(float x, float y, F&& func) {
			using R = xx::FuncR_t<F>;
			auto crIdx = PosToCrIdx(x, y);
			for (auto offset : offsets9) {
				auto col = crIdx.x + offset.x;
				if (col < 0 || col >= numCols) continue;
				auto row = crIdx.y + offset.y;
				if (row < 0 || row >= numRows) continue;
				auto cidx = CrIdxToCIdx(col, row);
				auto idx = cells[cidx];
				while (idx >= 0) {
					auto& o = ST::RefNode(idx);
					auto next = o.next;
					if constexpr (std::is_void_v<R>) {
						func(o.value);
					} else {
						auto r = func(o.value);
						if constexpr (std::is_same_v<R, bool>) {
							if (r) return;
						} else {
							switch (r) {
							case xx::ForeachResult::Continue: break;
							case xx::ForeachResult::Break: return;
							case xx::ForeachResult::RemoveAndContinue: {
								Remove(idx);
								break;
							}
							case xx::ForeachResult::RemoveAndBreak: {
								Remove(idx);
								return;
							}
							}
						}
					}
					idx = next;
				}
			}
		}

		template<typename F>
		void ForeachByRange(SpaceRingDiffuseData const& d, XY const& pos, float maxDistance, F&& func) {
			auto crIdx = PosToCrIdx(pos);					// calc grid col row index
			float rr = maxDistance * maxDistance;
			auto& lens = d.lens;
			auto& idxs = d.idxs;
			for (int i = 1; i < lens.len; i++) {

				auto offsets = &idxs[lens[i - 1].count];
				auto size = lens[i].count - lens[i - 1].count;

				for (int j = 0; j < size; ++j) {
					auto& offset = offsets[j];
					auto col = crIdx.x + offset.x;
					if (col < 0 || col >= numCols) continue;
					auto row = crIdx.y + offset.y;
					if (row < 0 || row >= numRows) continue;
					auto cidx = CrIdxToCIdx(col, row);
					CellForeach(cidx, [&](T& m)->void {
						auto v = m.pos - pos;
						if (v.x * v.x + v.y * v.y < rr) {
							func(m);		// todo: check func's args. send v, rr to func ?
						}
					});
				}

				if (lens[i].radius > maxDistance) break;			// limit search range
			}
		}

	};

}
