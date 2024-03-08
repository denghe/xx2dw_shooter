#pragma once
#include <game_looper.h>

// todo: intrusive grid container. only support memmoveable type

// Grid item weak pointer
struct GridsWeak {
	uint32_t typeId;			// Grids TS.index
	int32_t idx;				// Grids.gs[ typeIdx ].index
	uint32_t version;			// Grids.gs[ typeIdx ][ index ].version
};

// every derived class need member: " static constexpr uint32_t cTypeId{ ???? } "
template <class T>
concept Has_cTypeId = requires(T) { T::cTypeId; };

// all grid item's base class
struct Base {
	static constexpr uint32_t cTypeId{ (uint32_t) - 1};
	uint32_t version, typeId;
	int32_t next, prev, idx, cidx;
	XY pos;

	GridsWeak ToWeak() {
		return { typeId, idx, version };
	}
};

template<std::derived_from<Base> T>
struct Grid {
	static_assert(Has_cTypeId<T>);
	using T_t = T;

	T* buf{};
	int32_t cap{}, len{};
	int32_t freeHead{ -1 }, freeCount{};
	uint32_t version{};

	int32_t numRows{}, numCols{}, diameter{};
	int32_t* cells{};

	Grid() = default;
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

	void Init(int32_t const& numRows_, int32_t numCols_, int32_t diameter_) {
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
	T& Emplace(XY const& pos, Args&&...args) {
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
		o.~T();
		Free(idx);
	}

	void Remove(T const& o) {
		Remove(o.idx);
	}

	void Remove(GridsWeak const& gw) {
		if (Exists(gw)) {
			Remove(gw.idx);
		}
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

	bool Exists(GridsWeak const& gw) {
		assert(gw.typeId == T::cTypeId);
		assert(gw.version > 0);
		assert(gw.idx >= 0 && gw.idx < len);
		return buf[gw.idx].version == gw.version;
	}

	T& Get(GridsWeak const& gw) const {
		assert(Exists(gw));
		return (T&)buf[gw.idx];
	}

	// todo: search funcs
};

template<typename...TS>
struct Grids {
	using Tup = std::tuple<TS...>;
	static constexpr std::array<size_t, sizeof...(TS)> ss{ sizeof(TS)... };
	static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
	static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
	static_assert(ts == is);

	std::tuple<Grid<TS>...> gs;
	std::array<Grid<Base>*, sizeof...(TS)> gas;

	Grids() {
		// todo: addrs[TS::cTypeId] = (Grid<Base>*)gs[TS].buf ...;
	}
	Grids(Grids const&) = delete;
	Grids& operator=(Grids const&) = delete;

	template<typename T>
	Grid<T>& Get() const {
		return (Grid<T>&)std::get<Grid<T>>(gs);
	}

	// more forward funcs?

	template<typename T, typename ... Args>
	void Init(Args && ... args) {
		std::get<Grid<T>>(gs).Init(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	T& Emplace(XY const& pos, Args && ... args) {
		return std::get<Grid<T>>(gs).Emplace(pos, std::forward<Args>(args)...);
	}

	bool Exists(GridsWeak const& gw) {
		auto buf = ((Grid<Base>*)&std::get<0>(gs))[gw.typeId].buf;
		auto ptr = (char*)buf + ss[gw.typeId] * gw.idx;
		return ((Base*)ptr)->version == gw.version;
	}

	void Remove(GridsWeak const& gw) {
		// todo
	}
};


struct Foo : Base {
	static constexpr uint32_t cTypeId{ 3 };
	float radius{};
	void Init(float radius_) {
		radius = radius_;
	}
};

struct A : Base {
	static constexpr uint32_t cTypeId{ 0 };
	int aaa{};
	void Init() {}
};

struct B : Base {
	static constexpr uint32_t cTypeId{ 1 };
	std::string sss;
	void Init() {}
};

struct C : Base {
	static constexpr uint32_t cTypeId{ 2 };
	void Init() {}
};


struct SceneTest1 : Scene {
	inline static SceneTest1* instance{};			// init by Init()
	xx::Shared<Node> rootNode;
	Camera camera;

	Grid<Foo> grid;

	Grids<A, B, C> grids;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
