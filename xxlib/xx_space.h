#pragma once
#include "xx_blocklink.h"

namespace xx {
	template<typename T>
	struct SpaceNode : BlockLinkVI {
		int32_t nex, pre, cidx;
		T value;
	};

	struct SpaceCountRadius { int32_t count; float radius; };
	union SpaceIndex { 
		struct {
		int32_t x, y;
		};
		uint64_t data{};
	};
	struct SpaceXY { float x, y; };

	struct SpaceRingDiffuseData {
		List<SpaceCountRadius, int32_t> lens;
		List<SpaceIndex, int32_t> idxs;

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
	// T has member: float x, y
	template<typename T>
	struct SpaceGrid : protected BlockLink<T, SpaceNode> {
		using ST = BlockLink<T, SpaceNode>;
		using ST::ST;
		using Node = SpaceNode<T>;

		using ST::Count;
		using ST::Foreach;
		using ST::Reserve;
		using ST::TryGet;

		int32_t numRows{}, numCols{}, cellSize{};
		int32_t maxX{}, maxY{};
	protected:
		int32_t cellsLen{};
		std::unique_ptr<int32_t[]> cells;
	public:

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

		template<bool freeBuf = false, bool resetVersion = false>
		void Clear() {
			if (!cells) return;
			ST::template Clear<freeBuf, resetVersion>();
			memset(cells.get(), -1, sizeof(int32_t) * cellsLen);
		}

		// Emplace + Init( args ) + cells[ pos ] = o
		template<typename...Args>
		Node& EmplaceNodeInit(Args&&...args) {
			assert(cells);
			auto& o = ST::EmplaceCore();
			o.value.Init(std::forward<Args>(args)...);

			auto cidx = PosToCIdx(o.value.x, o.value.y);
			auto head = cells[cidx];	// backup
			if (head >= 0) {
				ST::RefNode(head).pre = o.index;
			}
			cells[cidx] = o.index;			// assign new
			o.nex = head;
			o.pre = -1;
			o.cidx = cidx;
			return o;
		}

		template<typename...Args>
		T& EmplaceInit(Args&&...args) {
			return EmplaceNodeInit(std::forward<Args>(args)...).value;
		}

	protected:
		XX_FORCE_INLINE void Free(Node& o) {
			assert(o.version < -1);
			assert(o.pre != o.index && o.nex != o.index && o.cidx >= 0);

			if (o.index == cells[o.cidx]) {
				cells[o.cidx] = o.nex;
			}
			if (o.pre >= 0) {
				ST::RefNode(o.pre).nex = o.nex;
			}
			if (o.nex >= 0) {
				ST::RefNode(o.nex).pre = o.pre;
			}
			//o.pre = -1;
			//o.cidx = -1;

			ST::Free(o);
		}
	public:

		void Remove(T const& v) {
			auto o = container_of(&v, Node, value);
			Free(o);
		}

		bool Remove(BlockLinkVI const& vi) {
			if (vi.version >= -1 || vi.index < 0 || vi.index >= this->len) return false;
			auto& o = ST::RefNode(vi.index);
			if (o.version != vi.version) return false;
			Free(o);
			return true;
		}

		void Update(T& v) {
			auto& o = *container_of(&v, Node, value);
			assert(o.index >= 0);
			assert(o.pre != o.index);
			assert(o.nex != o.index);
			auto cidx = PosToCIdx(v.x, v.y);
			if (cidx == o.cidx) return;				// no change

			// unlink
			if (o.index != cells[o.cidx]) {			// isn't head
				ST::RefNode(o.pre).nex = o.nex;
				if (o.nex >= 0) {
					ST::RefNode(o.nex).pre = o.pre;
					//o.nex = -1;
				}
				//o.pre = -1;
			} else {								// is head
				assert(o.pre == -1);
				cells[o.cidx] = o.nex;
				if (o.nex >= 0) {
					ST::RefNode(o.nex).pre = -1;
					//o.nex = -1;
				}
			}
			//o.cidx = -1;

			// relink
			if (cells[cidx] >= 0) {
				ST::RefNode(cells[cidx]).pre = o.index;
			}
			o.nex = cells[cidx];
			o.pre = -1;
			cells[cidx] = o.index;
			o.cidx = cidx;
		}

		// .Foreach([](T& o)->void {    });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		// return is Break or RemoveAndBreak
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		XX_FORCE_INLINE bool Foreach(int32_t cidx, F&& func) {
			auto idx = cells[cidx];
			while (idx >= 0) {
				auto& o = ST::RefNode(idx);
				auto nex = o.nex;
				if constexpr (std::is_void_v<R>) {
					func(o.value);
				} else {
					auto r = func(o.value);
					if constexpr (std::is_same_v<R, bool>) {
						if (r) return true;
					} else {
						switch (r) {
						case ForeachResult::Continue: break;
						case ForeachResult::RemoveAndContinue:
							Free(o);
							break;
						case ForeachResult::Break: return true;
						case ForeachResult::RemoveAndBreak:
							Free(o);
							return true;
						}
					}
				}
				idx = nex;
			}
			return false;
		}

		XX_FORCE_INLINE int32_t PosToCIdx(float x, float y) {
			assert(x >= 0 && x < cellSize * numCols);
			assert(y >= 0 && y < cellSize * numRows);
			auto c = int32_t(x) / cellSize;
			auto r = int32_t(y) / cellSize;
			return r * numCols + c;
		}

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

		// cell's index to pos( left top corner )
		XX_FORCE_INLINE XY CIdxToPos(int32_t cidx) {
			assert(cidx >= 0 && cidx < cellsLen);
			auto row = cidx / numCols;
			auto col = cidx - row * numCols;
			return { float(col * cellSize), float(row * cellSize) };
		}

		// cell's index to cell center pos
		XX_FORCE_INLINE XY CIdxToCenterPos(int32_t cidx) {
			return CIdxToPos(cidx) + float(cellSize) / 2;
		}

		XX_FORCE_INLINE XY CrIdxToPos(int32_t colIdx, int32_t rowIdx) {
			return CIdxToPos(rowIdx * numCols + colIdx);
		}

		XX_FORCE_INLINE XY CrIdxToCenterPos(int32_t colIdx, int32_t rowIdx) {
			return CIdxToCenterPos(rowIdx * numCols + colIdx);
		}

		constexpr static std::array<SpaceIndex, 9> offsets9 = { SpaceIndex
			{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
		};

		// foreach target cell + round 8 = 9 cells
		// .Foreach9([](T& o)->void {    });
		// .Foreach9([](T& o)->xx::ForeachResult {    });
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		void Foreach9(float x, float y, F&& func) {
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
					auto nex = o.nex;
					if constexpr (std::is_void_v<R>) {
						func(o.value);
					} else {
						auto r = func(o.value);
						if constexpr (std::is_same_v<R, bool>) {
							if (r) return;
						} else {
							switch (r) {
							case ForeachResult::Continue: break;
							case ForeachResult::RemoveAndContinue:
								Free(o);
								break;
							case ForeachResult::Break: return;
							case ForeachResult::RemoveAndBreak:
								Free(o);
								return;
							}
						}
					}
					idx = nex;
				}
			}
		}

		template<typename F, typename R = std::invoke_result_t<F, T&>>
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

					Foreach(cidx, [&](T& m)->void {
						auto v = m.pos - pos;
						if (v.x * v.x + v.y * v.y < rr) {
							func(m);		// todo: check func's args. send v, rr to func ?
						}
					});

				}

				if (lens[i].radius > maxDistance) break;			// limit search range
			}
		}


		//void Remove(SpaceWeak<T> const& w) {
		//	if (!w.Exists()) return;
		//	Remove(w.RefNode().index);
		//}

		//void Remove(int32_t index) {
		//	auto& o = ST::RefNode(index);
		//	Remove(o);
		//}


	};

}
