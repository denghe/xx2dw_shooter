#pragma once
#include <game_looper.h>

// todo: intrusive grid container. only support memmoveable type

struct Base {
	int32_t next, prev, idx, cidx;
	uint32_t version;			// disposed flag
	XY pos;
};

// todo: Pointer

//template<std::derived_from<Base> T>
struct Grid {
	using T = Base;

	int32_t* cells{};
	T* buf{};
	int32_t cap{}, len{};	// for buf
	int32_t freeHead{ -1 }, freeCount{};
	uint32_t version{};
	int32_t numRows{}, numCols{}, diameter{};	// for cells

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
		auto head = cells[cidx];
		if (head >= 0) {
			buf[head].prev = idx;
		}
		cells[cidx] = idx;

		auto& o = buf[idx];
		new (&o) T();
		o.next = head;
		o.prev = -1;
		o.idx = idx;
		o.cidx = cidx;
		o.version = GenVersion();
		o.pos = pos;
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
		o.next = -1;
		o.prev = -1;
		o.idx = -1;
		o.cidx = -1;
		o.version = 0;
		o.~T();
		Free(idx);
	}

	void Remove(T const& o) {
		Remove(o.idx);
	}

	void Update(T& o, XY const& newPos) {
		assert(o.idx >= 0);
		assert(o.prev != o.idx);
		assert(o.next != o.idx);
		auto cidx = PosToCIdx(newPos);
		if (cidx == o.cidx) return;				// no change
		o.pos = newPos;

		// unlink
		if (o.cidx != cells[o.cidx]) {			// isn't header
			assert(cells[o.cidx] != o.idx);
			buf[o.prev].next = o.next;
			if (o.next >= 0) {
				buf[o.next].prev = o.prev;
				//o.next = -1;
			}
			//o.prev = -1;
		} else {
			assert(cells[o.cidx] == o.idx);
			cells[o.cidx] = o.next;
			if (o.next >= 0) {
				buf[o.next].prev = -1;
				//o.next = -1;
			}
		}
		//o.cidx = -1;
		assert(cells[o.cidx] != o.idx);
		assert(cidx != o.cidx);

		// relink
		if (cells[cidx] >= 0) {
			buf[cells[cidx]].prev = o.idx;
		}
		o.next = cells[cidx];
		o.prev = -1;
		cells[cidx] = o.idx;
		o.cidx = cidx;

		assert(buf[cells[cidx]].prev == 0);
		assert(o.prev != o.idx);
		assert(o.next != o.idx);
	}

	// todo: search funcs
};



struct SceneTest1 : Scene {
	inline static SceneTest1* instance{};			// init by Init()
	xx::Shared<Node> rootNode;
	Camera camera;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};
