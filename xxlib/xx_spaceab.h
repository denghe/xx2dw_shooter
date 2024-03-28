#pragma once
#include "xx_blocklink.h"
#include "xx_xy.h"

namespace xx {

	template<typename T>
	struct SpaceABNode;

	template<typename T>
	struct SpaceABCell {
		SpaceABNode<T>* self;
		int32_t cidx;
		SpaceABCell* prev, * next;
	};

	template<typename T>
	struct SpaceABNode : BlockLinkVI {
		using CellType = SpaceABCell<T>;
		xx::FromTo<XYi> crIdx;
		std::vector<CellType> cs;	// todo: replace to int[] ? int[0] store len
		uint32_t flag;
		T value;

		static SpaceABNode& From(T& value) {
			return *container_of(&value, SpaceABNode, value);
		}
	};

	template<typename T>
	using SpaceABWeak = BlockLinkWeak<T, SpaceABNode>;

	// requires
	// T has member: xx::FromTo<XY> aabb;	// XY s2{ siz / 2 }; aabb.from = pos - s2; aabb.to = pos + s2;
	template<typename T, typename ST = BlockLink<T, SpaceABNode>>
	struct SpaceABGrid : protected ST {
		using ST::ST;
		using NodeType = typename ST::NodeType;
		using CellType = typename NodeType::CellType;

		using ST::Count;
		using ST::Reserve;
		using ST::TryGet;

		int32_t numRows{}, numCols{}, cellsLen{};
		XYi cellSize{}, max{};
	protected:
		std::unique_ptr<CellType* []> cells;
	public:
		Listi32<T*> results;

		void Init(int32_t numRows_, int32_t numCols_, XYi const& cellSize_) {
			assert(!cells);
			assert(numRows_ > 0 && numCols_ > 0 && cellSize_.x > 0 && cellSize_.y > 0);
			numRows = numRows_;
			numCols = numCols_;
			cellSize = cellSize_;
			max.x = cellSize_.x * numCols_;
			max.y = cellSize_.y * numRows_;

			cellsLen = numRows * numCols;
			cells = std::make_unique<CellType * []>(cellsLen);
			memset(cells.get(), 0, sizeof(CellType*) * cellsLen);
		}

		template<bool freeBuf = false, bool resetVersion = false>
		void Clear() {
			if (!cells) return;
			ST::template Clear<freeBuf, resetVersion>();
			memset(cells.get(), 0, sizeof(CellType*) * cellsLen);
		}

		static int32_t CalcNumCoveredCells(xx::FromTo<XYi> const& crIdx) {
			return (crIdx.to.x - crIdx.from.x + 1) * (crIdx.to.y - crIdx.from.y + 1);
		}

		// Emplace + Init( args ) + cells[?] = o.cs[?] ...
		template<typename...Args>
		NodeType& EmplaceNodeInit(Args&&...args) {
			assert(cells);
			auto& o = ST::EmplaceCore();
			auto& v = o.value;
			v.Init(std::forward<Args>(args)...);		// user need fill pos, aabb
			auto& ab = v.aabb;
			assert(ab.from.x < ab.to.x);
			assert(ab.from.y < ab.to.y);
			assert(ab.from.x >= 0 && ab.to.x < max.x);
			assert(ab.from.y >= 0 && ab.to.y < max.y);

			// calc covered cells
			xx::FromTo<XYi> crIdx{ ab.from / cellSize, ab.to / cellSize };
			o.crIdx = crIdx;
			auto numCoveredCells = CalcNumCoveredCells(crIdx);

			// link
			std::construct_at(&o.cs);
			o.cs.reserve(numCoveredCells);
			for (auto row = crIdx.from.y; row <= crIdx.to.y; row++) {
				for (auto col = crIdx.from.x; col <= crIdx.to.x; col++) {
					int32_t cidx = row * numCols + col;
					auto& head = cells[cidx];
					auto& c = o.cs.emplace_back(CellType{ &o, cidx, nullptr, head });
					if (head) {
						head->prev = &c;
					}
					head = &c;
				}
			}

			o.flag = {};
			return o;
		}

		template<typename...Args>
		T& EmplaceInit(Args&&...args) {
			return EmplaceNodeInit(std::forward<Args>(args)...).value;
		}

	protected:
		XX_FORCE_INLINE void Free(NodeType& o) {
			assert(o.cs.size());

			// unlink
			for (auto& c : o.cs) {
				if (c.prev) {	// isn't header
					c.prev->next = c.next;
					if (c.next) {
						c.next->prev = c.prev;
					}
				} else {
					cells[c.cidx] = c.next;
					if (c.next) {
						c.next->prev = {};
					}
				}
			}
			std::destroy_at(&o.cs);

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
			assert(!o.flag);

			// calc covered cells
			auto& ab = v.aabb;
			xx::FromTo<XYi> crIdx{ ab.from / cellSize, ab.to / cellSize };
			if (memcmp(&crIdx, &o.crIdx, sizeof(crIdx)) == 0) {
				auto numCoveredCells = CalcNumCoveredCells(crIdx);
				assert(numCoveredCells == o.cs.size());
				return;	// no change
			}
			o.crIdx = crIdx;

			// unlink
			for (auto& c : o.cs) {
				if (c.prev) {	// isn't header
					c.prev->next = c.next;
					if (c.next) {
						c.next->prev = c.prev;
					}
				} else {
					cells[c.cidx] = c.next;
					if (c.next) {
						c.next->prev = {};
					}
				}
			}
			o.cs.clear();

			// link
			for (auto row = crIdx.from.y; row <= crIdx.to.y; row++) {
				for (auto col = crIdx.from.x; col <= crIdx.to.x; col++) {
					int32_t cidx = row * numCols + col;
					auto& head = cells[cidx];
					auto& c = o.cs.emplace_back(CellType{ &o, cidx, nullptr, head });
					if (head) {
						head->prev = &c;
					}
					head = &c;
				}
			}
		}

		// todo: check func return value ?
		template<typename F>
		void ForeachPoint(XY const& p, F&& func) {
			XYi crIdx{ p / cellSize };
			if (crIdx.x < 0 || crIdx.x >= numCols || crIdx.y < 0 || crIdx.y >= numRows) return;
			auto c = cells[crIdx.y * numCols + crIdx.x];
			while (c) {
				auto next = c->next;
				auto& ab = *c->self->value.aabb;
				if (!(ab.to.x < p.x || p.x < ab.from.x || ab.to.y < p.y || p.y < ab.from.y)) {
					func(c->value);
				}
				c = next;
			}
		}

		void ClearResults() {
			for (auto& o : results) {
				o->flag = 0;
			}
			results.Clear();
		}

		// fill items to results. need ClearResults()
		// auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
		template<bool enableLimit = false, bool enableExcept = false>
		void ForeachAABB(xx::FromTo<XY> const& ab, int32_t* limit = nullptr, T* except = nullptr) {
			assert(ab.from.x < ab.to.x);
			assert(ab.from.y < ab.to.y);
			assert(ab.from.x >= 0 && ab.from.y >= 0);
			assert(ab.to.x < max.x && ab.to.y < max.y);
			assert(results.Empty());

			// except set flag
			if constexpr (enableExcept) {
				assert(except);
				except->flag = 1;
			}

			// calc covered cells
			xx::FromTo<XYi> crIdx{ ab.from / cellSize, ab.to / cellSize };

			if (crIdx.from.x == crIdx.to.x || crIdx.from.y == crIdx.to.y) {
				// 1-2 row, 1-2 col: do full rect check
				for (auto rIdx = crIdx.from.y; rIdx <= crIdx.to.y; rIdx++) {
					for (auto cIdx = crIdx.from.x; cIdx <= crIdx.to.x; cIdx++) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							auto& sab = s->value.aabb;
							if (!(sab.to.x < ab.from.x || ab.to.x < sab.from.x || sab.to.y < ab.from.y || ab.to.y < sab.from.y)) {
								if (!s->flag) {
									s->flag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (--*limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}
			} else {
				// first row: check AB
				auto rIdx = crIdx.from.y;

				// first cell: check top left AB
				auto cIdx = crIdx.from.x;
				auto c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					auto& sab = s->value.aabb;
					if (sab.to.x > ab.from.x && sab.to.y > ab.from.y) {
						if (!s->flag) {
							s->flag = 1;
							results.push_back(s);
						}
						if constexpr (enableLimit) {
							if (--*limit == 0) break;
						}
					}
					c = c->next;
				}

				// middle cells: check top AB
				for (++cIdx; cIdx < crIdx.to.x; cIdx++) {
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						auto& sab = s->value.aabb;
						if (sab.to.y > ab.from.y) {
							if (!s->flag) {
								s->flag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (--*limit == 0) break;
							}
						}
						c = c->next;
					}
				}

				// last cell: check top right AB
				if (cIdx == crIdx.to.x) {
					auto c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						auto& sab = s->value.aabb;
						if (sab.from.x < ab.to.x && sab.to.y > ab.from.y) {
							if (!s->flag) {
								s->flag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (--*limit == 0) break;
							}
						}
						c = c->next;
					}
				}

				// middle rows: first & last col check AB
				for (++rIdx; rIdx < crIdx.to.y; rIdx++) {

					// first cell: check left AB
					cIdx = crIdx.from.x;
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						auto& sab = s->value.aabb;
						if (sab.to.x > ab.from.x) {
							if (!s->flag) {
								s->flag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (--*limit == 0) break;
							}
						}
						c = c->next;
					}

					// middle cols: no check
					for (; cIdx < crIdx.to.x; cIdx++) {
						c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							if (!s->flag) {
								s->flag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (--*limit == 0) break;
							}
							c = c->next;
						}
					}

					// last cell: check right AB
					if (cIdx == crIdx.to.x) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							auto& sab = s->value.aabb;
							if (sab.from.x < ab.to.x) {
								if (!s->flag) {
									s->flag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (--*limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}

				// last row: check AB
				if (rIdx == crIdx.to.y) {

					// first cell: check left bottom AB
					cIdx = crIdx.from.x;
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						auto& sab = s->value.aabb;
						if (sab.to.x > ab.from.x && sab.from.y < ab.to.y) {
							if (!s->flag) {
								s->flag = 1;
								results.push_back(s);
							}
							if constexpr (enableLimit) {
								if (--*limit == 0) break;
							}
						}
						c = c->next;
					}

					// middle cells: check bottom AB
					for (++cIdx; cIdx < crIdx.to.x; cIdx++) {
						c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							auto& sab = s->value.aabb;
							if (sab.from.y < ab.to.y) {
								if (!s->flag) {
									s->flag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (--*limit == 0) break;
								}
							}
							c = c->next;
						}
					}

					// last cell: check right bottom AB
					if (cIdx == crIdx.to.x) {
						auto c = cells[rIdx * numCols + cIdx];
						while (c) {
							auto&& s = c->self;
							auto& sab = s->value.aabb;
							if (sab.from.x < ab.to.x && sab.from.y < ab.to.y) {
								if (!s->flag) {
									s->flag = 1;
									results.push_back(s);
								}
								if constexpr (enableLimit) {
									if (--*limit == 0) break;
								}
							}
							c = c->next;
						}
					}
				}
			}

			// except clear flag
			if constexpr (enableExcept) {
				assert(except);
				except->flag = 0;
			}

		}
	};

}






//template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
//struct FixedListPool;

//// copy code fom List ( maybe combine with List ? )
//template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
//struct FixedListCore {
//	typedef T ChildType;
//	using Pool = FixedListPool<T, SizeType, cap>;

//	Pool* pool;
//	SizeType len;
//	T buf[0];

//	FixedListCore() = delete;
//	FixedListCore(FixedListCore const& o) = delete;
//	FixedListCore& operator=(FixedListCore const& o) = delete;
//	~FixedListCore() noexcept {
//		Clear();
//	}

//	template<typename...Args>
//	T& Emplace(Args&&...args) {
//		assert(len < cap);
//		return *new (&buf[len++]) T(std::forward<Args>(args)...);
//	}

//	template<typename ...TS>
//	void Add(TS&&...vs) {
//		(Emplace(std::forward<TS>(vs)), ...);
//	}

//	void RemoveAt(SizeType idx) {
//		assert(idx >= 0 && idx < len);
//		--len;
//		if constexpr (IsPod_v<T>) {
//			buf[idx].~T();
//			::memmove(buf + idx, buf + idx + 1, (len - idx) * sizeof(T));
//		} else {
//			for (SizeType i = idx; i < len; ++i) {
//				buf[i] = (T&&)buf[i + 1];
//			}
//			buf[len].~T();
//		}
//	}

//	void SwapRemoveAt(SizeType idx) {
//		assert(idx >= 0 && idx < len);
//		buf[idx].~T();
//		--len;
//		if (len != idx) {
//			if constexpr (IsPod_v<T>) {
//				::memcpy(&buf[idx], &buf[len], sizeof(T));
//			} else {
//				new (&buf[idx]) T((T&&)buf[len]);
//			}
//		}
//	}

//	void Clear() {
//		if (!cap) return;
//		if (len) {
//			for (SizeType i = 0; i < len; ++i) {
//				buf[i].~T();
//			}
//			len = 0;
//		}
//	}

//	T const& operator[](SizeType idx) const noexcept {
//		assert(idx >= 0 && idx < len);
//		return buf[idx];
//	}

//	T& operator[](SizeType idx) noexcept {
//		assert(idx >= 0 && idx < len);
//		return buf[idx];
//	}

//	struct Iter {
//		T* ptr;
//		bool operator!=(Iter const& other) noexcept { return ptr != other.ptr; }
//		Iter& operator++() noexcept { ++ptr; return *this; }
//		T& operator*() noexcept { return *ptr; }
//	};
//	Iter begin() noexcept { return Iter{ buf }; }
//	Iter end() noexcept { return Iter{ buf + len }; }
//	Iter begin() const noexcept { return Iter{ buf }; }
//	Iter end() const noexcept { return Iter{ buf + len }; }
//};


//// wrapper
//template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
//struct FixedList {
//	using Pool = FixedListPool<T, SizeType, cap>;
//	using Core = FixedListCore<T, SizeType, cap>;

//	Core* core{};

//	FixedList() = default;
//	FixedList(FixedList const& o) = delete;
//	FixedList& operator=(FixedList const& o) = delete;
//	FixedList(FixedList&& o) : core(std::exchange(o.core, nullptr)) {};
//	FixedList& operator=(FixedList&& o) {
//		swap(core, o.core);
//		return *this;
//	}

//	void Alloc(Pool& owner);

//	~FixedList() noexcept {
//		if (core) core->Clear();
//		// todo: release core from pool
//	}

//	template<typename...Args>
//	T& Emplace(Args&&...args) {
//		assert(core);
//		return core->Emplace(std::forward<Args>(args)...);
//	}

//	template<typename ...TS>
//	void Add(TS&&...vs) {
//		assert(core);
//		(core->Emplace(std::forward<TS>(vs)), ...);
//	}

//	void RemoveAt(SizeType idx) {
//		assert(core);
//		core->RemoveAt(idx);
//	}

//	void SwapRemoveAt(SizeType idx) {
//		assert(core);
//		core->SwapRemoveAt(idx);
//	}

//	void Clear() {
//		assert(core);
//		core->Clear();
//	}

//	T const& operator[](SizeType idx) const noexcept {
//		assert(core);
//		return core->operator[](idx);
//	}

//	T& operator[](SizeType idx) noexcept {
//		assert(core);
//		return core->operator[](idx);
//	}

//	Core::Iter begin() noexcept { assert(core); return core->begin(); }
//	Core::Iter end() noexcept { assert(core); return core->end(); }
//	Core::Iter begin() const noexcept { assert(core); return core->begin(); }
//	Core::Iter end() const noexcept { assert(core); return core->end(); }
//};

//template<typename T, typename SizeType, SizeType cap>
//struct FixedListPool {
//	using Node = FixedList<T, SizeType>;
//	static constexpr size_t nodeSize = sizeof(Node) + sizeof(T) * cap;
//	static constexpr size_t blockSize = nodeSize * 64;

//	xx::Listi32<void*> blocks;
//	int32_t cap{}, len{}, freeHead{ -1 }, freeCount{};

//	XX_FORCE_INLINE void Reserve() {
//		cap += 64;
//		blocks.Emplace(malloc(blockSize));
//	}

//	//XX_FORCE_INLINE void* RefBlock(int32_t index) const {
//	//	assert(index >= 0 && index < this->cap);
//	//	return *(Block*)this->blocks[(uint32_t&)index >> 6];
//	//}

//	//XX_FORCE_INLINE Node<T>& RefNode(int32_t index) const {
//	//	auto& block = RefBlock(index);
//	//	auto& node = block.buf[index & 0b111111];
//	//	assert(node.index == index);
//	//	return node;
//	//}


//	// todo
//};

//template<typename T, typename SizeType, SizeType cap>
//void FixedList<T, SizeType, cap>::Alloc(Pool& owner) {

//}



