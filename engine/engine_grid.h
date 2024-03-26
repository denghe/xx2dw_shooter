#pragma once
#include <engine_prims.h>

// intrusive grid container

// todo: replace base code to blocklist

#pragma region Weak

// single Grid item weak pointer
struct GridWeak {
	int32_t idx{ -1 };
	uint32_t version{};

	void Reset() {
		idx = -1;
		version = 0;
	}
	bool Empty() const {
		return version == 0;
	}
};

// Grids item weak pointer
struct GridsWeak : GridWeak {
	int32_t typeId{ -1 };

	void Reset() {
		GridWeak::Reset();
		typeId = -1;
	}
};

struct GridWeakCount : GridWeak {
	int32_t count{};
};

struct GridsWeakCount : GridsWeak {
	int32_t count{};
};

namespace xx {
	template<>
	struct IsPod<GridWeak, void> : std::true_type {};
	template<>
	struct IsPod<GridsWeak, void> : std::true_type {};
	template<>
	struct IsPod<GridWeakCount, void> : std::true_type {};
	template<>
	struct IsPod<GridsWeakCount, void> : std::true_type {};
}

#pragma endregion

#pragma region ItemBase

// every derived class need member: " static constexpr uint32_t cTypeId{ ???? } "
template <class T>
concept Has_cTypeId = requires(T) { T::cTypeId; };

// all grid item's base class
struct GridItemBase {
	static constexpr int32_t cTypeId{ -1 };
	uint32_t __grid_version;
	int32_t typeId, __grid_next, __grid_prev, __grid_idx, __grid_cidx;
	XY pos;

	GridWeak ToGridWeak() {
		return { __grid_idx, __grid_version};
	}

	GridsWeak ToGridsWeak() {
		return { {__grid_idx, __grid_version}, typeId };
	}
};

#pragma endregion

#pragma region Grid


//#define GRID_ENABLE_CELL_FLAG

// single grid ( space index by pos )
template<std::derived_from<GridItemBase> T>
struct Grid {
	static_assert(Has_cTypeId<T>);
	using T_t = T;

	T* buf{};
	int32_t cap{}, len{};
	int32_t freeHead{ -1 }, freeCount{};
	uint32_t version{};

	int32_t numRows{}, numCols{}, cellSize{};
	int32_t maxX{}, maxY{};		// fill by init
	int32_t cellsLen{};			// fill by init
	int32_t bufFlagsLen{};		// fill by init

#ifdef GRID_ENABLE_CELL_FLAG
	int32_t cellFlagsLen{};		// fill by init
	size_t* cellFlags{};		// cells empty flag. has value: 1. when foreach, fast skip empty cells
#endif

	size_t* bufFlags{};			// buf empty flag. has value: 1. when foreach, fast skip empty buf. length == cap / 4/8 + 1 ??
	int32_t* cells{};


	Grid() = default;
	Grid(int32_t numRows_, int32_t numCols_, int32_t cellSize_) : Grid() {
		Init(numRows_, numCols_, cellSize_);
	}
	Grid(Grid const&) = delete;
	Grid& operator=(Grid const&) = delete;

#ifdef GRID_ENABLE_CELL_FLAG
	void CellFlagSet(int32_t cidx) {
		auto fidx = cidx / (sizeof(size_t) * 8);
		auto bit = size_t(1) << (cidx - (sizeof(size_t) * 8) * fidx);
		assert((cellFlags[fidx] & bit) == 0);
		cellFlags[fidx] |= bit;
	}

	void CellFlagUnset(int32_t cidx) {
		auto fidx = cidx / (sizeof(size_t) * 8);
		auto bit = size_t(1) << (cidx - (sizeof(size_t) * 8) * fidx);
		assert(((cellFlags[fidx] & bit) ^ bit) == 0);
		cellFlags[fidx] &= ~bit;
	}

	// grid.CellsForeach([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
	template<typename F>
	void CellsForeach(F&& func) {
		using R = xx::FuncR_t<F>;
		static_assert(std::is_same_v<R, xx::ForeachResult> || std::is_same_v<R, bool> || std::is_void_v<R>);
		for (int32_t i = 0; i < cellFlagsLen; ++i) {
			if (!cellFlags[i]) continue;
			auto b = i * sizeof(size_t) * 8;
			auto e = std::min(b + sizeof(size_t) * 8, (size_t)cellsLen);
			for (; b < e; ++b) {
				auto node = cells[b];
				while (node >= 0) {
					auto next = buf[node].next;
					if constexpr (std::is_void_v<R>) {
						func(buf[node]);
					} else {
						auto r = func(buf[node]);
						if constexpr (std::is_same_v<R, bool>) {
							if (r) return;
						} else {
							switch (r) {
							case xx::ForeachResult::Continue: break;
							case xx::ForeachResult::Break: return;
							case xx::ForeachResult::RemoveAndContinue: {
								Remove(node);
								break;
							}
							case xx::ForeachResult::RemoveAndBreak: {
								Remove(node);
								return;
							}
							}
						}
					}
					node = next;
				}
			}
		}
	}
#endif

	void BufFlagSet(int32_t idx) {
		auto fidx = idx / (sizeof(size_t) * 8);
		auto bit = size_t(1) << (idx - (sizeof(size_t) * 8) * fidx);
		assert((bufFlags[fidx] & bit) == 0);
		bufFlags[fidx] |= bit;
	}

	void BufFlagUnset(int32_t idx) {
		auto fidx = idx / (sizeof(size_t) * 8);
		auto bit = size_t(1) << (idx - (sizeof(size_t) * 8) * fidx);
		assert(((bufFlags[fidx] & bit) ^ bit) == 0);
		bufFlags[fidx] &= ~bit;
	}

	// grid.BufForeach([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
	template<typename F>
	void BufForeach(F&& func) {
		using R = xx::FuncR_t<F>;
		static_assert(std::is_same_v<R, xx::ForeachResult> || std::is_same_v<R, bool> || std::is_void_v<R>);
		if (len <= 0) return;
		auto bflen = (len - 1) / sizeof(size_t) + 1;
		for (size_t i = 0; i < bflen; ++i) {
			if (!bufFlags[i]) continue;
			auto b = i * sizeof(size_t) * 8;
			auto e = std::min(b + sizeof(size_t) * 8, (size_t)len);
			for (; b < e; ++b) {
				auto& o = buf[b];
				if (!o.__grid_version) continue;
				if constexpr (std::is_void_v<R>) {
					func(o);
				} else {
					auto r = func(o);
					if constexpr (std::is_same_v<R, bool>) {
						if (r) return;
					} else {
						switch (r) {
						case xx::ForeachResult::Continue: break;
						case xx::ForeachResult::Break: return;
						case xx::ForeachResult::RemoveAndContinue: {
							Remove((int32_t)b);
							break;
						}
						case xx::ForeachResult::RemoveAndBreak: {
							Remove((int32_t)b);
							return;
						}
						}
					}
				}
			}
		}
	}

	// grid.CellForeach([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
	template<typename F>
	void CellForeach(int32_t cidx, F&& func) {
		using R = xx::FuncR_t<F>;
		auto idx = cells[cidx];
		while (idx >= 0) {
			auto next = buf[idx].__grid_next;
			if constexpr (std::is_void_v<R>) {
				func(buf[idx]);
			} else {
				auto r = func(buf[idx]);
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


	~Grid() {
		BufForeach([](T& o) {
			o.~T();
		});
		::free(buf);
		buf = {};
		if (cells) {
			free(cells);
			cells = {};
		}
		if (bufFlags) {
			free(bufFlags);
			bufFlags = {};
		}
#ifdef GRID_ENABLE_CELL_FLAG
		if (cellFlags) {
			free(cellFlags);
			cellFlags = {};
		}
#endif
	}

	template<bool resetVersion = false>
	void Clear() {
		if (!cells) return;

		BufForeach([](T& o) {
			o.~T();
		});
		len = 0;
		freeHead = -1;
		freeCount = 0;
		if constexpr (resetVersion) {
			version = 0;
		}

		memset(bufFlags, 0, bufFlagsLen * sizeof(size_t));

		auto numBytes = sizeof(int32_t) * cellsLen;
		memset(cells, -1, numBytes);	// -1 mean empty

#ifdef GRID_ENABLE_CELL_FLAG
		numBytes = cellFlagsLen * sizeof(size_t);
		memset(cellFlags, 0, numBytes);
#endif
	}

	void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		assert(!cells);
		numRows = numRows_;
		numCols = numCols_;
		cellSize = cellSize_;
		maxX = cellSize_ * numCols_;
		maxY = cellSize_ * numRows_;

		cellsLen = numRows * numCols;
		assert(cellsLen);
		auto numBytes = sizeof(int32_t) * cellsLen;
		cells = (int32_t*)malloc(numBytes);
		memset(cells, -1, numBytes);	// -1 mean empty

#ifdef GRID_ENABLE_CELL_FLAG
		cellFlagsLen = (cellsLen - 1) / sizeof(size_t) + 1;
		numBytes = cellFlagsLen * sizeof(size_t);
		cellFlags = (size_t*)malloc(numBytes);
		memset(cellFlags, 0, numBytes);
#endif
	}

	void Reserve(int32_t newCap) noexcept {
		assert(newCap > 0);
		if (newCap <= cap) return;
		cap = xx::Round2n(newCap);
		auto bak = bufFlagsLen;
		bufFlagsLen = (newCap - 1) / sizeof(size_t) + 1;
		bufFlags = (size_t*)realloc(bufFlags, sizeof(size_t) * bufFlagsLen);
		memset(bufFlags + bak, 0, (bufFlagsLen - bak) * sizeof(size_t));
		cap = newCap;
		if constexpr (xx::IsPod_v<T>) {
			buf = (T*)realloc(buf, sizeof(T) * newCap);
		} else {
			auto newBuf = (T*)::malloc(sizeof(T) * newCap);
			for (int32_t i = 0; i < len; ++i) {
				new (&newBuf[i]) T((T&&)buf[i]);
				buf[i].~T();
			}
			::free(buf);
			buf = newBuf;
		}
	}

	int32_t Count() const {
		return len - freeCount;
	}

	uint32_t GenVersion() {
		if (version == std::numeric_limits<uint32_t>().max()) {
			version = 1;
		} else {
			++version;
		}
		return version;
	}

	int32_t Alloc() {
		int32_t idx;
		if (freeCount > 0) {
			idx = freeHead;
			freeHead = buf[idx].__grid_next;
			freeCount--;
		} else {
			if (len == cap) {
				Reserve(cap ? cap * 2 : 8);
			}
			idx = len;
			len++;
		}
		BufFlagSet(idx);
		return idx;
	}

	void Free(int32_t idx) {
		auto& o = buf[idx];
		o.__grid_next = freeHead;
		o.__grid_version = 0;
		freeHead = idx;
		++freeCount;
		BufFlagUnset(idx);
	}

	T& Make(XY const& pos = {}) {
		auto cidx = PosToCIdx(pos);
		auto idx = Alloc();
		auto head = cells[cidx];	// backup
		if (head >= 0) {
			buf[head].prev = idx;
		} else {
#ifdef GRID_ENABLE_CELL_FLAG
			CellFlagSet(cidx);
#endif
		}
		cells[cidx] = idx;	// assign new

		auto& o = buf[idx];
		new (&o) T();
		o.__grid_version = GenVersion();
		o.typeId = T::cTypeId;
		o.__grid_next = head;	// write backup to o
		o.__grid_prev = -1;
		o.__grid_idx = idx;
		o.__grid_cidx = cidx;
		o.pos = pos;
		return o;
	}

	// o need Init func ( need fill pos )
	template<typename...Args>
	T& MakeInit(Args&&...args) {
		auto idx = Alloc();
		auto& o = buf[idx];
		new (&o) T();

		o.__grid_version = GenVersion();
		o.typeId = T::cTypeId;
		o.__grid_idx = idx;

		o.Init(std::forward<Args>(args)...);	// need fill pos here

		auto cidx = PosToCIdx(o.pos);
		auto head = cells[cidx];	// backup
		if (head >= 0) {
			buf[head].__grid_prev = idx;
		} else {
#ifdef GRID_ENABLE_CELL_FLAG
			CellFlagSet(cidx);
#endif
		}
		cells[cidx] = idx;	// assign new
		o.__grid_next = head;	// write backup to o
		o.__grid_prev = -1;
		o.__grid_cidx = cidx;
		return o;
	}

	void Remove(int32_t idx) {
		assert(idx >= 0 && idx < len);
		auto& o = buf[idx];
		assert(o.__grid_prev != o.__grid_idx && o.__grid_next != o.__grid_idx && o.__grid_version && o.__grid_cidx >= 0 && o.__grid_idx == idx);
		if (idx == cells[o.__grid_cidx]) {
			cells[o.__grid_cidx] = o.__grid_next;
			if (o.__grid_next == -1) {
#ifdef GRID_ENABLE_CELL_FLAG
				CellFlagUnset(o.__grid_cidx);
#endif
			}
		}
		if (o.__grid_prev >= 0) {
			buf[o.__grid_prev].__grid_next = o.__grid_next;
		}
		if (o.__grid_next >= 0) {
			buf[o.__grid_next].__grid_prev = o.__grid_prev;
		}
		o.__grid_version = 0;
		//o.typeId = -1;
		o.__grid_next = -1;
		o.__grid_prev = -1;
		o.__grid_idx = -1;
		o.__grid_cidx = -1;
		if constexpr (!std::is_same_v<GridItemBase, T>) {		// for Grids call. let Grids use deleter destruct
			o.~T();
		}
		Free(idx);
	}

	void Remove(T const& o) {
		Remove(o.__grid_idx);
	}

	void Remove(GridWeak const& gw) {
		assert(Exists(gw));
		Remove(gw.idx);
	}

	bool Exists(GridWeak const& gw) const {
		if (gw.version == 0) return false;
		assert(gw.idx >= 0 && gw.idx < len);
		return buf[gw.idx].__grid_version == gw.version;
	}

	T& Get(GridWeak const& gw) const {
		assert(Exists(gw));
		return (T&)buf[gw.idx];
	}

	T* TryGet(GridWeak const& gw) const {
		if (!Exists(gw)) return nullptr;
		return &(T&)buf[gw.idx];
	}

	void Update(T& o, XY const& newPos) {
		assert(o.__grid_idx >= 0);
		assert(o.__grid_prev != o.__grid_idx);
		assert(o.__grid_next != o.__grid_idx);
		o.pos = newPos;
		auto cidx = PosToCIdx(newPos);
		if (cidx == o.__grid_cidx) return;				// no change

		// unlink
		if (o.__grid_idx != cells[o.__grid_cidx]) {			// isn't header
			buf[o.__grid_prev].__grid_next = o.__grid_next;
			if (o.__grid_next >= 0) {
				buf[o.__grid_next].__grid_prev = o.__grid_prev;
				//o.__grid_next = -1;
			}
			//o.__grid_prev = -1;
		} else {
			assert(o.__grid_prev == -1);
			cells[o.__grid_cidx] = o.__grid_next;
			if (o.__grid_next >= 0) {
				buf[o.__grid_next].__grid_prev = -1;
				//o.next = -1;
			} else {
#ifdef GRID_ENABLE_CELL_FLAG
				CellFlagUnset(o.cidx);
#endif
			}
		}
		//o.cidx = -1;

		// relink
		if (cells[cidx] >= 0) {
			buf[cells[cidx]].__grid_prev = o.__grid_idx;
		} else {
#ifdef GRID_ENABLE_CELL_FLAG
			CellFlagSet(cidx);
#endif
		}
		o.__grid_next = cells[cidx];
		o.__grid_prev = -1;
		cells[cidx] = o.__grid_idx;
		o.__grid_cidx = cidx;
	}

	XX_FORCE_INLINE int32_t PosToCIdx(XY const& pos) {
		assert(pos.x >= 0 && pos.x < cellSize * numCols);
		assert(pos.y >= 0 && pos.y < cellSize * numRows);
		auto c = int32_t(pos.x) / cellSize;
		auto r = int32_t(pos.y) / cellSize;
		return r * numCols + c;
	}

	// return x: col index   y: row index
	XX_FORCE_INLINE XYi PosToCrIdx(XY const& pos) {
		assert(pos.x >= 0 && pos.x < cellSize * numCols);
		assert(pos.y >= 0 && pos.y < cellSize * numRows);
		return { int32_t(pos.x) / cellSize, int32_t(pos.y) / cellSize };
	}

	// return cell's index
	XX_FORCE_INLINE int32_t CrIdxToCIdx(XYi const& crIdx) {
		return crIdx.y * numCols + crIdx.x;
	}

	constexpr static std::array<XYi, 9> offsets9 = { XYi
		{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
	};

	// foreach target cell + round 8 = 9 cells
	// grid.Foreach9([](T& o)->xx::ForeachResult { o...; return xx::ForeachResult::xxxxx; });
	template<typename F>
	void Foreach9(XY const& pos, F&& func) {
		using R = xx::FuncR_t<F>;
		auto crIdx = PosToCrIdx(pos);	
		for (auto& offset : offsets9) {
			auto cr = crIdx + offset;
			if (cr.x < 0 || cr.x >= numCols || cr.y < 0 || cr.y >= numRows) continue;
			auto cidx = CrIdxToCIdx(cr);
			auto idx = cells[cidx];
			while (idx >= 0) {
				auto next = buf[idx].__grid_next;
				if constexpr (std::is_void_v<R>) {
					func(buf[idx]);
				} else {
					auto r = func(buf[idx]);
					if constexpr (std::is_same_v<R, bool>) {
						if (r) return;
					} else {
						switch (r) {
						case xx::ForeachResult::Continue: break;
						case xx::ForeachResult::Break: return;
						case xx::ForeachResult::RemoveAndContinue:
						{
							Remove(idx);
							break;
						}
						case xx::ForeachResult::RemoveAndBreak:
						{
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
	void ForeachByRange(SpaceGridRingDiffuseData const& sgrdd, XY const& pos, float maxDistance, F&& func) {
		auto crIdx = PosToCrIdx(pos);					// calc grid col row index
		float rr = maxDistance * maxDistance;
		auto& lens = sgrdd.lens;
		auto& idxs = sgrdd.idxs;
		for (int i = 1; i < lens.len; i++) {

			auto offsets = &idxs[lens[i - 1].count];
			auto size = lens[i].count - lens[i - 1].count;

			for (int j = 0; j < size; ++j) {
				auto cr = crIdx + offsets[j];
				if (cr.x < 0 || cr.x >= numCols || cr.y < 0 || cr.y >= numRows) continue;
				auto cidx = CrIdxToCIdx(cr);
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

	T* TryGetCellItemByPos(XY const& p) {
		if (p.x < 0 || p.x >= maxX || p.y < 0 || p.y >= maxX) return nullptr;
		auto cidx = PosToCIdx(p);
		auto idx = cells[cidx];
		if (idx < 0) return nullptr;
		return &buf[idx];
	}

	T* TryGetCellItemByCrIdx(XYi crIdx) {
		if (crIdx.x < 0 || crIdx.x >= numCols || crIdx.y < 0 || crIdx.y >= numRows) return nullptr;
		auto cidx = CrIdxToCIdx(crIdx);
		auto idx = cells[cidx];
		if (idx < 0) return nullptr;
		return &buf[idx];
	}

	T* TryGetNext(T* item) {
		if (!item || item->__grid_next == -1) return nullptr;
		return &buf[item->__grid_next];
	}

	// todo: more search funcs

	// todo: Save Load
};

#pragma endregion

#pragma region Grids

// grid groups( cross visit by GridsWeak )
template<typename...TS>
struct Grids {
	using Tup = std::tuple<TS...>;
	static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
	static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
	static_assert(ts == is);

	// container
	xx::SimpleTuple<Grid<TS>...> gs;
	static_assert(sizeof(gs.value) * sizeof...(TS) == sizeof(gs));

	// sizes for calculate offsets
	static constexpr std::array<size_t, sizeof...(TS)> sizes{ sizeof(TS)... };

	// for fast Remove weak type
	typedef void(*Deleter)(void*);
	std::array<Deleter, sizeof...(TS)> deleters;

	typedef GridItemBase*(*Caster)(void*);
	std::array<Caster, sizeof...(TS)> casters;


	Grids() {
		xx::ForEachType<Tup>([&]<typename T>() {
			deleters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { ((T*)o)->~T(); };
			casters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { return (GridItemBase*)(T*)o; };
		});
	}
	Grids(Grids const&) = delete;
	Grids& operator=(Grids const&) = delete;

	template<typename T>
	Grid<T>& Get() const {
		return xx::Get<Grid<T>>(gs);
	}

	template<typename ...US>
	void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		xx::ForEachType<Tup>([&]<typename T>() {
			Get<T>().Init(numRows_, numCols_, cellSize_);
		});
	}

	void InitAll(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		Init<TS...>(numRows_, numCols_, cellSize_);
	}

	template<typename T>
	T& Make(XY const& pos) {
		return Get<T>().Make(pos);
	}

	template<typename T, typename ... Args>
	T& MakeInit(Args && ... args) {
		return Get<T>().MakeInit(std::forward<Args>(args)...);
	}

	template<typename T = void>
	void Remove(GridsWeak const& gw) {
		if constexpr (std::is_void_v<T>) {
			auto& g = ((Grid<GridItemBase>*) & gs)[gw.typeId];
			auto& o = g.Get(gw);
			g.Remove(gw);
			deleters[gw.typeId](&o);
		} else {
			assert(gw.typeId == T::cTypeId);
			Get<T>().Remove(gw);
		}
	}

	template<typename T = void>
	bool Exists(GridsWeak const& gw) {
		if (gw.version == 0 || gw.typeId == -1 || gw.idx == -1) return false;
		if constexpr (std::is_void_v<T>) {
			auto buf = ((Grid<GridItemBase>*) & gs)[gw.typeId].buf;
			auto ptr = (char*)buf + sizes[gw.typeId] * gw.idx;
			return ((GridItemBase*)ptr)->__grid_version == gw.version;
		} else {
			return Get<T>().Exists(gw);
		}
	}

	template<typename T = void>
	std::conditional_t< std::is_void_v<T>, GridItemBase*, T* > TryGetBase(GridsWeak const& gw) {
		if (gw.version == 0 || gw.typeId == -1 || gw.idx == -1) return nullptr;
		if constexpr (std::is_void_v<T>) {
			auto buf = ((Grid<GridItemBase>*) & gs)[gw.typeId].buf;
			auto ptr = casters[gw.typeId]((char*)buf + sizes[gw.typeId] * gw.idx);
			if (ptr->__grid_version != gw.version) return nullptr;
			return ptr;
		} else {
			return Get<T>().TryGet(gw);
		}
	}

	template<typename T>
	void Update(T& o, XY const& newPos) {
		Get<T>().Update(o, newPos);
	}

	// grids.Foreach([&]<typename T>(Grid<T>&grid) { ...... });
	template<typename F>
	void ForeachAll(F&& func) {
		(func(Get<TS>()), ...);
	}


	int32_t Count() {
		return (Get<TS>().Count() + ...);
	}
	// more forward funcs?

	// todo: Save Load
};

#pragma endregion

#pragma region GridsEx

// Grids + user define BaseType
template<std::derived_from<GridItemBase> BT, std::derived_from<BT>...TS>
struct GridsEx {
	using Tup = std::tuple<TS...>;
	static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
	static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
	static_assert(ts == is);

	// container
	xx::SimpleTuple<Grid<TS>...> gs;
	static_assert(sizeof(gs.value) * sizeof...(TS) == sizeof(gs));

	// sizes for calculate offsets
	static constexpr std::array<size_t, sizeof...(TS)> sizes{ sizeof(TS)... };

	// for fast Remove weak type
	typedef void(*Deleter)(void*);
	std::array<Deleter, sizeof...(TS)> deleters;

	typedef BT* (*Caster)(void*);
	std::array<Caster, sizeof...(TS)> casters;


	GridsEx() {
		xx::ForEachType<Tup>([&]<typename T>() {
			deleters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { ((T*)o)->~T(); };
			casters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { return (BT*)(T*)o; };
		});
	}
	GridsEx(GridsEx const&) = delete;
	GridsEx& operator=(GridsEx const&) = delete;

	template<typename T>
	Grid<T>& Get() const {
		return xx::Get<Grid<T>>((xx::SimpleTuple<Grid<TS>...>&)gs);
	}

	template<typename ...US>
	void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		xx::ForEachType<Tup>([&]<typename T>() {
			Get<T>().Init(numRows_, numCols_, cellSize_);
		});
	}

	void InitAll(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		Init<TS...>(numRows_, numCols_, cellSize_);
	}

	template<typename T>
	T& Make(XY const& pos) {
		return Get<T>().Make(pos);
	}

	template<typename T, typename ... Args>
	T& MakeInit(Args && ... args) {
		return Get<T>().MakeInit(std::forward<Args>(args)...);
	}

	template<typename T = void>
	void Remove(GridsWeak const& gw) {
		if constexpr (std::is_void_v<T>) {
			auto& g = ((Grid<BT>*) & gs)[gw.typeId];
			auto& o = g.Get(gw);	// todo: test
			g.Remove(gw);
			deleters[gw.typeId](&o);
		} else {
			assert(gw.typeId == T::cTypeId);
			Get<T>().Remove(gw);
		}
	}

	template<typename T = void>
	bool Exists(GridsWeak const& gw) {
		if (gw.version == 0 || gw.typeId == -1 || gw.idx == -1) return false;
		if constexpr (std::is_void_v<T>) {
			auto buf = ((Grid<BT>*) & gs)[gw.typeId].buf;
			auto ptr = casters[gw.typeId]((char*)buf + sizes[gw.typeId] * gw.idx);
			return ptr->__grid_version == gw.version;
		} else {
			return Get<T>().Exists(gw);
		}
	}

	template<typename T = void>
	std::conditional_t< std::is_void_v<T>, BT*, T* > TryGetBase(GridsWeak const& gw) {
		if (gw.version == 0 || gw.typeId == -1 || gw.idx == -1) return nullptr;
		if constexpr (std::is_void_v<T>) {
			auto buf = ((Grid<BT>*) & gs)[gw.typeId].buf;
			auto ptr = casters[gw.typeId]((char*)buf + sizes[gw.typeId] * gw.idx);
			if (ptr->__grid_version != gw.version) return nullptr;
			return ptr;
		} else {
			return Get<T>().TryGet(gw);
		}
	}

	template<typename T>
	void Update(T& o, XY const& newPos) {
		Get<T>().Update(o, newPos);
	}

	// grids.Foreach([&]<typename T>(Grid<T>&grid) { ...... });
	template<typename F>
	void ForeachAll(F&& func) {
		(func(Get<TS>()), ...);
	}


	int32_t Count() {
		return (Get<TS>().Count() + ...);
	}
	// more forward funcs?

	// todo: Save Load
};

#pragma endregion


#pragma region Append

namespace xx {
	template<typename T>
	struct StringFuncs<T, std::enable_if_t<std::is_same_v<::GridWeak, T>>> {
		static inline void Append(std::string& s, T const& in) {
			::xx::Append(s, "GridWeak { "
				, "idx = ", in.idx
				, ", version = ", in.version
				, " }"
			);
		}
	};

	template<typename T>
	struct StringFuncs<T, std::enable_if_t<std::is_same_v<::GridsWeak, T>>> {
		static inline void Append(std::string& s, T const& in) {
			::xx::Append(s, "GridsWeak { "
				, "idx = ", in.idx
				, ", version = ", in.version
				, ", typeId = ", in.typeId
				, " }"
			);
		}
	};

	template<typename T>
	struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<::GridItemBase, T>>> {
		static inline void Append(std::string& s, T const& in) {
			if (in.version == 0) {
				::xx::Append(s, "GridItemBase { null }");
			} else {
				::xx::Append(s, "GridItemBase { "
					, "version = ", in.__grid_version
					, ", typeId = ", in.typeId
					, ", next = ", in.__grid_next
					, ", prev = ", in.__grid_prev
					, ", idx = ", in.__grid_idx
					, ", cidx = ", in.__grid_cidx
					, ", pos = ", in.pos
					, " }"
				);
			}
		}
	};
}

#pragma endregion
