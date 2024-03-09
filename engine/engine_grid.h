﻿#pragma once
#include <engine_prims.h>

// intrusive grid container

#pragma region Weak

// single Grid item weak pointer
struct GridWeak {
	int32_t idx;
	uint32_t version;
};

// Grids item weak pointer
struct GridsWeak : GridWeak {
	uint32_t typeId;
};

#pragma endregion

#pragma region ItemBase

// every derived class need member: " static constexpr uint32_t cTypeId{ ???? } "
template <class T>
concept Has_cTypeId = requires(T) { T::cTypeId; };

// all grid item's base class
struct GridItemBase {
	static constexpr uint32_t cTypeId{ (uint32_t)-1 };
	uint32_t version, typeId;
	int32_t next, prev, idx, cidx;
	XY pos;

	GridWeak ToGridWeak() {
		return {idx, version};
	}

	GridsWeak ToGridsWeak() {
		return { {idx, version}, typeId };
	}
};

enum class GridForeachResult {
	Continue,
	Break,
	RemoveAndContinue,
	RemoveAndReturn
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

	// grid.CellsForeach([](T& o)->GridForeachResult { o...; return GridForeachResult::xxxxx; });
	template<typename F>
	void CellsForeach(F&& func) {
		using R = xx::FuncR_t<F>;
		static_assert(std::is_same_v<R, GridForeachResult> || std::is_same_v<R, bool> || std::is_void_v<R>);
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
							case GridForeachResult::Continue: break;
							case GridForeachResult::Break: return;
							case GridForeachResult::RemoveAndContinue: {
								Remove(node);
								break;
							}
							case GridForeachResult::RemoveAndReturn: {
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

	// grid.BufForeach([](T& o)->GridForeachResult { o...; return GridForeachResult::xxxxx; });
	template<typename F>
	void BufForeach(F&& func) {
		using R = xx::FuncR_t<F>;
		static_assert(std::is_same_v<R, GridForeachResult> || std::is_same_v<R, bool> || std::is_void_v<R>);
		if (len <= 0) return;
		auto bflen = (len - 1) / sizeof(size_t) + 1;
		for (size_t i = 0; i < bflen; ++i) {
			if (!bufFlags[i]) continue;
			auto b = i * sizeof(size_t) * 8;
			auto e = std::min(b + sizeof(size_t) * 8, (size_t)len);
			for (; b < e; ++b) {
				auto& o = buf[b];
				if (!o.version) continue;
				if constexpr (std::is_void_v<R>) {
					func(o);
				} else {
					auto r = func(o);
					if constexpr (std::is_same_v<R, bool>) {
						if (r) return;
					} else {
						switch (r) {
						case GridForeachResult::Continue: break;
						case GridForeachResult::Break: return;
						case GridForeachResult::RemoveAndContinue: {
							Remove((int32_t)b);
							break;
						}
						case GridForeachResult::RemoveAndReturn: {
							Remove((int32_t)b);
							return;
						}
						}
					}
				}
			}
		}
	}

	// grid.CellForeach([](T& o)->GridForeachResult { o...; return GridForeachResult::xxxxx; });
	template<typename F>
	void CellForeach(int32_t cidx, F&& func) {
		using R = xx::FuncR_t<F>;
		auto idx = cells[cidx];
		while (idx >= 0) {
			auto next = buf[idx].next;
			if constexpr (std::is_void_v<R>) {
				func(buf[idx]);
			} else {
				auto r = func(buf[idx]);
				if constexpr (std::is_same_v<R, bool>) {
					if (r) return;
				} else {
					switch (r) {
					case GridForeachResult::Continue: break;
					case GridForeachResult::Break: return;
					case GridForeachResult::RemoveAndContinue: {
						Remove(idx);
						break;
					}
					case GridForeachResult::RemoveAndReturn: {
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

	// todo: Clear ?

	void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
		assert(!cells);
		numRows = numRows_;
		numCols = numCols_;
		cellSize = cellSize_;

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

	int32_t PosToCIdx(XY const& pos) {
		assert(pos.x >= 0 && pos.x < cellSize * numCols);
		assert(pos.y >= 0 && pos.y < cellSize * numRows);
		auto c = int32_t(pos.x) / cellSize;
		auto r = int32_t(pos.y) / cellSize;
		return r * numCols + c;
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
			freeHead = buf[idx].next;
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
		o.next = freeHead;
		o.version = 0;
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
		o.version = GenVersion();
		o.typeId = T::cTypeId;
		o.next = head;	// write backup to o
		o.prev = -1;
		o.idx = idx;
		o.cidx = cidx;
		o.pos = pos;
		return o;
	}

	// o need Init func ( need fill pos )
	template<typename...Args>
	T& MakeInit(Args&&...args) {
		auto idx = Alloc();
		auto& o = buf[idx];
		new (&o) T();

		o.version = GenVersion();
		o.typeId = T::cTypeId;
		o.idx = idx;

		o.Init(std::forward<Args>(args)...);	// need fill pos here

		auto cidx = PosToCIdx(o.pos);
		auto head = cells[cidx];	// backup
		if (head >= 0) {
			buf[head].prev = idx;
		} else {
#ifdef GRID_ENABLE_CELL_FLAG
			CellFlagSet(cidx);
#endif
		}
		cells[cidx] = idx;	// assign new
		o.next = head;	// write backup to o
		o.prev = -1;
		o.cidx = cidx;
		return o;
	}

	void Remove(int32_t idx) {
		assert(idx >= 0 && idx < len);
		auto& o = buf[idx];
		assert(o.prev != o.idx && o.next != o.idx && o.version && o.cidx >= 0 && o.idx == idx);
		if (idx == cells[o.cidx]) {
			cells[o.cidx] = o.next;
			if (o.next == -1) {
#ifdef GRID_ENABLE_CELL_FLAG
				CellFlagUnset(o.cidx);
#endif
			}
		}
		if (o.prev >= 0) {
			buf[o.prev].next = o.next;
		}
		if (o.next >= 0) {
			buf[o.next].prev = o.prev;
		}
		o.version = 0;
		//o.typeId = -1;
		o.next = -1;
		o.prev = -1;
		o.idx = -1;
		o.cidx = -1;
		if constexpr (!std::is_same_v<GridItemBase, T>) {		// for Grids call. let Grids use deleter destruct
			o.~T();
		}
		Free(idx);
	}

	void Remove(T const& o) {
		Remove(o.idx);
	}

	void Remove(GridWeak const& gw) {
		assert(Exists(gw));
		Remove(gw.idx);
	}

	bool Exists(GridWeak const& gw) const {
		assert(gw.version > 0);
		assert(gw.idx >= 0 && gw.idx < len);
		return buf[gw.idx].version == gw.version;
	}

	T& Get(GridWeak const& gw) const {
		assert(Exists(gw));
		return (T&)buf[gw.idx];
	}

	void Update(T& o, XY const& newPos) {
		assert(o.idx >= 0);
		assert(o.prev != o.idx);
		assert(o.next != o.idx);
		o.pos = newPos;
		auto cidx = PosToCIdx(newPos);
		if (cidx == o.cidx) return;				// no change

		// unlink
		if (o.idx != cells[o.cidx]) {			// isn't header
			buf[o.prev].next = o.next;
			if (o.next >= 0) {
				buf[o.next].prev = o.prev;
				//o.next = -1;
			}
			//o.prev = -1;
		} else {
			assert(o.prev == -1);
			cells[o.cidx] = o.next;
			if (o.next >= 0) {
				buf[o.next].prev = -1;
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
			buf[cells[cidx]].prev = o.idx;
		} else {
#ifdef GRID_ENABLE_CELL_FLAG
			CellFlagSet(cidx);
#endif
		}
		o.next = cells[cidx];
		o.prev = -1;
		cells[cidx] = o.idx;
		o.cidx = cidx;
	}

	// todo: fast foreach. use 64bit flag skip empty cell ?

	// todo: search funcs

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

	Grids() {
		xx::ForEachType<Tup>([&]<typename T>() {
			deleters[xx::TupleTypeIndex_v<T, Tup>] = [](void* o) { ((T*)o)->~T(); };
		});
	}
	Grids(Grids const&) = delete;
	Grids& operator=(Grids const&) = delete;

	template<typename T>
	Grid<T>& Get() const {
		return ((Grid<T>*) & gs)[xx::TupleTypeIndex_v<T, Tup>];
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
		if constexpr (std::is_void_v<T>) {
			auto buf = ((Grid<GridItemBase>*) & gs)[gw.typeId].buf;
			auto ptr = (char*)buf + sizes[gw.typeId] * gw.idx;
			return ((GridItemBase*)ptr)->version == gw.version;
		} else {
			return Get<T>().Exists(gw);
		}
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
					, "version = ", in.version
					, ", typeId = ", in.typeId
					, ", next = ", in.next
					, ", prev = ", in.prev
					, ", idx = ", in.idx
					, ", cidx = ", in.cidx
					, ", pos = ", in.pos
					, " }"
				);
			}
		}
	};
}

#pragma endregion

/*

useage examples:


struct Foo : GridItemBase {
	static constexpr uint32_t cTypeId{ 3 };
	float radius{};
	void Init(float radius_) {
		radius = radius_;
	}
};

struct A : GridItemBase {
	static constexpr uint32_t cTypeId{ 0 };
	int aaa{};
	void Init() {
		xx::CoutN("A.Init()");
	}
	~A() {
		xx::CoutN("~A()");
	}
};

struct B : GridItemBase {
	static constexpr uint32_t cTypeId{ 1 };
	std::string sss;
	void Init() {
		xx::CoutN("B.Init()");
	}
	~B() {
		xx::CoutN("~B()");
	}
};

struct C : GridItemBase {
	static constexpr uint32_t cTypeId{ 2 };
	void Init() {
		xx::CoutN("C.Init()");
	}
	~C() {
		xx::CoutN("~C()");
	}
};

struct D : GridItemBase {
	static constexpr uint32_t cTypeId{ 0 };
	int val{ 1 };
	void Update() {};
};



	Grid<Foo> grid;

	Grids<A, B, C> grids;



	grid.Init(2, 3, 10);
	xx::CoutN("************************ 1");

	auto& foo1 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("************************ 2");

	auto& foo2 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("************************ 3");

	auto& foo3 = grid.MakeInit({ 5,5 }, 3.f);
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("foo3: ", foo3);
	xx::CoutN("************************ 4");

	//grid.Update(foo, { 15,15 });
	//xx::CoutN(foo.idx, "  ", foo.cidx, "  ", foo.pos);

	grid.Update(foo2, { 15,15 });
	xx::CoutN("foo1: ", foo1);
	xx::CoutN("foo2: ", foo2);
	xx::CoutN("foo3: ", foo3);
	xx::CoutN("************************ 5");



	grids.InitAll(1, 1, 1);

	auto& b = grids.MakeInit<B>({});
	xx::CoutN("b = ", b);
	auto p = b.ToGridsWeak();
	xx::CoutN("p = ", p);
	xx::CoutN(grids.Exists(p));
	grids.Remove(p);
	xx::CoutN(grids.Exists(p));
	xx::CoutN("b = ", b);



	Grid<D> dGrid(10000, 10000, 32);
	dGrid.Reserve(110000);
	for (size_t i = 0; i < 100000; i++) {
		auto x = gLooper.rnd.Next(dGrid.numCols - 1) * 16.f;
		auto y = gLooper.rnd.Next(dGrid.numRows - 1) * 16.f;
		dGrid.Make({ x, y });
	}

	dGrid.BufForeach([&](D& o)->GridForeachResult {
		return o.idx < 99999 ? GridForeachResult::RemoveAndContinue : GridForeachResult::Break;
	});

	auto secs = xx::NowEpochSeconds();
	int counter = 0;
	dGrid.BufForeach([&](D& o)->void {
		counter += o.val;
	});
	xx::CoutN(xx::NowEpochSeconds(secs), " counter = ", counter);
	counter = 0;
	for (int i = 0, e = dGrid.len; i < e; ++i) {
		if (auto& o = dGrid.buf[i]; o.version > 0) {
			counter += o.val;
		}
	}
	xx::CoutN(xx::NowEpochSeconds(secs), " counter = ", counter);

*/
