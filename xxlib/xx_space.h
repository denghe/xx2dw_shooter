#pragma once
#include "xx_blocklink.h"
#include "xx_xy.h"

namespace xx {
	template<typename T>
	struct SpaceNode : BlockLinkVI {
		int32_t nex, pre, cidx;
		T value;
	};

	struct SpaceCountRadius { int32_t count; float radius; };

	struct SpaceRingDiffuseData {
		Listi32<SpaceCountRadius> lens;
		Listi32<XYi> idxs;

		void Init(int32_t crCount, int32_t cSize) {
			auto step = (float)cSize / 2;
			lens.Emplace(0, 0.f);
			XYi lastIdx{};
			idxs.Add(lastIdx);
			Listi32<char> idxflags;
			auto n = crCount * 2;
			idxflags.Resize<true, 0>(n * n);
			XYi center{ n / 2, n / 2 };
			for (float r = step; r < cSize * crCount; r += step) {
				auto c = 2 * M_PI * r;
				if (c < step) continue;
				auto lenBak = idxs.len;
				auto astep = float(M_PI * 2 * (step / c) / 10);
				auto rd = r / cSize;
				for (float a = astep; a < M_PI * 2; a += astep) {
					XYi idx{ rd * std::cos(a), rd * std::sin(a) };
					if (lastIdx != idx) {
						if (auto i = (center.y + idx.y) * crCount + (center.x + idx.x); !idxflags[i]) {
							idxflags[i] = 1;
							idxs.Add(idx);
							lastIdx = idx;
						}
					}
				}
				if (idxs.len > lenBak) {
					lens.Emplace(idxs.len, r);
				}
			}
		}
	};

	template<typename T>
	using SpaceWeak = BlockLinkWeak<T, SpaceNode>;

	// requires
	// T has member: XY pos
	template<typename T, typename ST = BlockLink<T, SpaceNode>>
	struct SpaceGrid : protected ST {
		using ST::ST;
		using NodeType = typename ST::NodeType;

		using ST::Count;
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
		NodeType& EmplaceNodeInit(Args&&...args) {
			assert(cells);
			auto& o = ST::EmplaceCore();
			o.value.Init(std::forward<Args>(args)...);

			auto cidx = PosToCIdx(o.value.pos);
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
		XX_FORCE_INLINE void Free(NodeType& o) {
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
			auto o = container_of(&v, NodeType, value);
			Free(*o);
		}

		bool Remove(BlockLinkVI const& vi) {
			if (vi.version >= -2 || vi.index < 0 || vi.index >= this->len) return false;
			auto& o = ST::RefNode(vi.index);
			if (o.version != vi.version) return false;
			Free(o);
			return true;
		}

		void Update(T& v) {
			auto& o = *container_of(&v, NodeType, value);
			assert(o.index >= 0);
			assert(o.pre != o.index);
			assert(o.nex != o.index);
			auto cidx = PosToCIdx(v.pos);
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

		// foreach by flags ( copy ForeachFlags to here )
		// .Foreach([](T& o)->void {    });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		void Foreach(F&& func) {
			if (ST::len <= 0) return;

			for (int32_t i = 0, n = ST::blocks.len - 1; i <= n; ++i) {

				auto& block = *(typename ST::Block*)ST::blocks[i];
				auto& flags = block.flags;
				if (!flags) continue;

				auto left = ST::len & 0b111111;
				int32_t e = (i < n || !left) ? 64 : left;
				for (int32_t j = 0; j < e; ++j) {
					auto& o = block.buf[j];
					auto bit = uint64_t(1) << j;
					if ((flags & bit) == 0) {
						assert(o.version >= -2);
						continue;
					}
					assert(o.version < -2);

					if constexpr (std::is_void_v<R>) {
						func(o.value);
					} else {
						auto r = func(o.value);
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
			}
		}

		// .Foreach9([](T& o)->void {  all  });
		// .Foreach([](T& o)->bool {  break  });
		// .Foreach([](T& o)->xx::ForeachResult {    });
		// return is Break or RemoveAndBreak
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		XX_FORCE_INLINE bool ForeachCell(int32_t cidx, F&& func) {
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

		XX_FORCE_INLINE int32_t PosToCIdx(XYf const& p) {
			assert(p.x >= 0 && p.x < cellSize * numCols);
			assert(p.y >= 0 && p.y < cellSize * numRows);
			auto c = int32_t(p.x) / cellSize;
			auto r = int32_t(p.y) / cellSize;
			return r * numCols + c;
		}

		// return x: col index   y: row index
		XX_FORCE_INLINE XYi PosToCrIdx(XYf const& p) {
			assert(p.x >= 0 && p.x < cellSize * numCols);
			assert(p.y >= 0 && p.y < cellSize * numRows);
			return { int32_t(p.x) / cellSize, int32_t(p.y) / cellSize };
		}

		// return cell's index
		XX_FORCE_INLINE int32_t CrIdxToCIdx(XYi const& crIdx) {
			return crIdx.y * numCols + crIdx.x;
		}

		// cell's index to pos( left top corner )
		XX_FORCE_INLINE XYf CIdxToPos(int32_t cidx) {
			assert(cidx >= 0 && cidx < cellsLen);
			auto row = cidx / numCols;
			auto col = cidx - row * numCols;
			return { float(col * cellSize), float(row * cellSize) };
		}

		// cell's index to cell center pos
		XX_FORCE_INLINE XYf CIdxToCenterPos(int32_t cidx) {
			return CIdxToPos(cidx) + float(cellSize) / 2;
		}

		XX_FORCE_INLINE XYf CrIdxToPos(int32_t colIdx, int32_t rowIdx) {
			return CIdxToPos(rowIdx * numCols + colIdx);
		}

		XX_FORCE_INLINE XYf CrIdxToCenterPos(int32_t colIdx, int32_t rowIdx) {
			return CIdxToCenterPos(rowIdx * numCols + colIdx);
		}

		T* TryGetCellItemByPos(XY const& p) {
			if (p.x < 0 || p.x >= maxX || p.y < 0 || p.y >= maxX) return nullptr;
			auto cidx = PosToCIdx(p);
			auto idx = cells[cidx];
			if (idx < 0) return nullptr;
			return &ST::RefNode(idx).value;
		}

		constexpr static std::array<XYi, 9> offsets9 = { XYi
			{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
		};

		// foreach target cell + round 8 = 9 cells
		// .Foreach9([](T& o)->void {  all  });
		// .Foreach([](T& o)->bool {  break  });
		// .Foreach9([](T& o)->xx::ForeachResult {    });
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		void Foreach9(XYf const& pos, F&& func) {
			auto crIdxBase = PosToCrIdx(pos);
			for (auto offset : offsets9) {
				auto crIdx = crIdxBase + offset;
				if (crIdx.x < 0 || crIdx.x >= numCols) continue;
				if (crIdx.y < 0 || crIdx.y >= numRows) continue;
				auto cidx = CrIdxToCIdx(crIdx);

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

		// can't break
		template<typename F, typename R = std::invoke_result_t<F, T&>>
		void ForeachByRange(SpaceRingDiffuseData const& d, XYf const& pos, float maxDistance, F&& func) {
			auto crIdxBase = PosToCrIdx(pos);			// calc grid col row index
			float rr = maxDistance * maxDistance;
			auto& lens = d.lens;
			auto& idxs = d.idxs;
			for (int i = 1; i < lens.len; i++) {

				auto offsets = &idxs[lens[i - 1].count];
				auto size = lens[i].count - lens[i - 1].count;

				for (int j = 0; j < size; ++j) {
					auto crIdx = crIdxBase + offsets[j];
					if (crIdx.x < 0 || crIdx.x >= numCols) continue;
					if (crIdx.y < 0 || crIdx.y >= numRows) continue;
					auto cidx = CrIdxToCIdx(crIdx);
					ForeachCell(cidx, [&](T& m)->void {
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
