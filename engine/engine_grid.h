#pragma once
#include <engine_prims.h>

// intrusive grid container. only support memmoveable type

// every derived class need member: " static constexpr uint32_t cTypeId{ ???? } "
template <class T>
concept Has_cTypeId = requires(T) { T::cTypeId; };

// single Grid item weak pointer
struct GridWeak {
	int32_t idx;
	uint32_t version;
};

// Grids item weak pointer
struct GridsWeak : GridWeak {
	uint32_t typeId;
};

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

namespace xx {
	template<typename T>
	struct StringFuncs<T, std::enable_if_t<std::is_base_of_v<GridItemBase, T>>> {
		static inline void Append(std::string& s, T const& in) {
			::xx::Append(s, "Derived From GridItemBase{ "
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
	};
}

// single grid ( space index by pos )
template<std::derived_from<GridItemBase> T>
struct Grid {
	static_assert(Has_cTypeId<T>);
	using T_t = T;
	typedef void(*Deleter)(void*);

	T* buf{};
	int32_t cap{}, len{};
	int32_t freeHead{ -1 }, freeCount{};
	uint32_t version{};

	int32_t numRows{}, numCols{}, diameter{};
	int32_t* cells{};
	Deleter deleter{};	// for T == GridItemBase ( weak type mode )

	Grid() {
		deleter = [](void* o) { ((T*)o)->~T(); };
	}
	Grid(Grid const&) = delete;
	Grid& operator=(Grid const&) = delete;

	~Grid() {
		if (cells) {
			free(cells);
			cells = {};
		}
		for (int32_t i = 0; i < len; ++i) {
			if (auto& o = buf[i]; o.version) {
				o.version = 0;
				o.~T();
			}
		}
		::free(buf);
		buf = {};
	}

	void Init(int32_t numRows_, int32_t numCols_, int32_t diameter_) {
		assert(!cells);
		numRows = numRows_;
		numCols = numCols_;
		diameter = diameter_;
		auto n = sizeof(int32_t) * numRows * numCols;
		cells = (int32_t*)malloc(n);
		memset(cells, -1, n);	// -1 mean empty
	}

	XX_FORCE_INLINE void Reserve(int32_t newCap) noexcept {
		assert(newCap > 0);
		if (newCap <= cap) return;
		cap = newCap;
		buf = (T*)realloc(buf, sizeof(T) * newCap);
	}

	XX_FORCE_INLINE int32_t Count() const {
		return len - freeCount;
	}

	XX_FORCE_INLINE int32_t PosToCIdx(XY const& pos) {
		assert(pos.x >= 0 && pos.x < diameter * numCols);
		assert(pos.y >= 0 && pos.y < diameter * numRows);
		auto c = int32_t(pos.x) / diameter;
		auto r = int32_t(pos.y) / diameter;
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

	XX_FORCE_INLINE int32_t Alloc() {
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
		return idx;
	}

	XX_FORCE_INLINE void Free(int32_t idx) {
		auto& o = buf[idx];
		o.next = freeHead;
		o.version = 0;
		freeHead = idx;
		++freeCount;
	}

	T& Add(XY const& pos) {
		auto cidx = PosToCIdx(pos);
		auto idx = Alloc();
		auto head = cells[cidx];	// backup
		if (head >= 0) {
			buf[head].prev = idx;
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

	// o need Init func
	template<typename...Args>
	T& AddInit(XY const& pos, Args&&...args) {
		auto& o = Add(pos);
		o.Init(std::forward<Args>(args)...);
		return o;
	}

	void Remove(int32_t idx) {
		assert(idx >= 0 && idx < len);
		auto& o = buf[idx];
		assert(o.prev != o.idx && o.next != o.idx && o.version && o.cidx >= 0 && o.idx == idx);
		if (idx == cells[o.cidx]) {
			cells[o.cidx] = o.next;
		}
		if (o.prev >= 0) {
			buf[o.prev].next = o.next;
		}
		if (o.next >= 0) {
			buf[o.next].prev = o.prev;
		}
		o.version = 0;
		//o.typeId = 
		o.next = -1;
		o.prev = -1;
		o.idx = -1;
		o.cidx = -1;
		if constexpr (std::is_same_v<GridItemBase, T>) {
			deleter(&o);
		} else {
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
		auto cidx = PosToCIdx(newPos);
		if (cidx == o.cidx) return;				// no change
		o.pos = newPos;

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
			}
		}
		//o.cidx = -1;

		// relink
		if (cells[cidx] >= 0) {
			buf[cells[cidx]].prev = o.idx;
		}
		o.next = cells[cidx];
		o.prev = -1;
		cells[cidx] = o.idx;
		o.cidx = cidx;
	}

	// todo: search funcs

	// todo: Save Load
};

// grid groups( cross visit by GridsWeak )
template<typename...TS>
struct Grids {
	using Tup = std::tuple<TS...>;

	static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
	static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
	static_assert(ts == is);
	static_assert(ts[0] == 0);

	// container
	xx::SimpleTuple<Grid<TS>...> gs;
	static_assert(sizeof(gs.value) * sizeof...(TS) == sizeof(gs));

	// sizes for calculate offsets
	static constexpr std::array<size_t, sizeof...(TS)> sizes{ sizeof(TS)... };

	Grids() = default;
	Grids(Grids const&) = delete;
	Grids& operator=(Grids const&) = delete;

	template<typename T>
	Grid<T>& Get() const {
		return ((Grid<T>*) & gs)[xx::TupleTypeIndex_v<T, Tup>];
	}

	template<typename ...US>
	void Init(int32_t numRows_, int32_t numCols_, int32_t diameter_) {
		xx::ForEachType<Tup>([&]<typename T>() {
			Get<T>().Init(numRows_, numCols_, diameter_);
		});
	}

	void InitAll(int32_t numRows_, int32_t numCols_, int32_t diameter_) {
		Init<TS...>(numRows_, numCols_, diameter_);
	}

	template<typename T, typename ... Args>
	T& AddInit(XY const& pos, Args && ... args) {
		return Get<T>().AddInit(pos, std::forward<Args>(args)...);
	}

	template<typename T = void>
	void Remove(GridsWeak const& gw) {
		if constexpr (std::is_void_v<T>) {
			auto& g = ((Grid<GridItemBase>*) & gs)[gw.typeId];
			g.Remove(gw);
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
