#pragma once
#include "xx_blocklink.h"
#include "xx_tinylist.h"
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
		TinyList<CellType> cs;
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

		int32_t numRows{}, numCols{};// dummy == cellsLen
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

			ST::dummy = numRows * numCols;
			cells = std::make_unique<CellType * []>(ST::dummy);
			memset(cells.get(), 0, sizeof(CellType*) * ST::dummy);
		}

		template<bool freeBuf = false, bool resetVersion = false>
		void Clear() {
			if (!cells) return;
			ST::template Clear<freeBuf, resetVersion>();
			memset(cells.get(), 0, sizeof(CellType*) * ST::dummy);
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

			// calc covered cells( max value )
			xx::FromTo<XYi> crIdx{ ab.from / cellSize, ab.to / cellSize };
			o.crIdx = crIdx;
			auto numReserve = (crIdx.to.x - crIdx.from.x + 2) * (crIdx.to.y - crIdx.from.y + 2);

			// link
			std::construct_at(&o.cs);
			o.cs.Reserve(numReserve);
			for (auto row = crIdx.from.y; row <= crIdx.to.y; row++) {
				for (auto col = crIdx.from.x; col <= crIdx.to.x; col++) {
					int32_t cidx = row * numCols + col;
					auto& head = cells[cidx];
					auto& c = o.cs.Emplace();
					c.self = &o;
					c.cidx = cidx;
					c.prev = nullptr;
					c.next = head;
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
			assert(o.cs.Len());

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
			if (memcmp(&crIdx, &o.crIdx, sizeof(crIdx)) == 0) return;	// no change
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
			o.cs.Clear();

			// link
			for (auto row = crIdx.from.y; row <= crIdx.to.y; row++) {
				for (auto col = crIdx.from.x; col <= crIdx.to.x; col++) {
					int32_t cidx = row * numCols + col;
					auto& head = cells[cidx];
					auto& c = o.cs.Emplace();
					c.self = &o;
					c.cidx = cidx;
					c.prev = nullptr;
					c.next = head;
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
				auto& v = c->self->value;
				auto& ab = v.aabb;
				if (!(ab.to.x < p.x || p.x < ab.from.x || ab.to.y < p.y || p.y < ab.from.y)) {
					func(v);
				}
				c = next;
			}
		}

		void ClearResults() {
			for (auto v : results) {
				container_of(v, NodeType, value)->flag = 0;
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
							auto s = c->self;
							auto& v = s->value;
							auto& sab = v.aabb;
							if (!(sab.to.x < ab.from.x || ab.to.x < sab.from.x || sab.to.y < ab.from.y || ab.to.y < sab.from.y)) {
								if (!s->flag) {
									s->flag = 1;
									results.Emplace(&v);
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
					auto s = c->self;
					auto& v = s->value;
					auto& sab = v.aabb;
					if (sab.to.x > ab.from.x && sab.to.y > ab.from.y) {
						if (!s->flag) {
							s->flag = 1;
							results.Emplace(&v);
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
						auto s = c->self;
						auto& v = s->value;
						auto& sab = v.aabb;
						if (sab.to.y > ab.from.y) {
							if (!s->flag) {
								s->flag = 1;
								results.Emplace(&v);
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
						auto s = c->self;
						auto& v = s->value;
						auto& sab = v.aabb;
						if (sab.from.x < ab.to.x && sab.to.y > ab.from.y) {
							if (!s->flag) {
								s->flag = 1;
								results.Emplace(&v);
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
						auto s = c->self;
						auto& v = s->value;
						auto& sab = v.aabb;
						if (sab.to.x > ab.from.x) {
							if (!s->flag) {
								s->flag = 1;
								results.Emplace(&v);
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
							auto s = c->self;
							if (!s->flag) {
								s->flag = 1;
								results.Emplace(&s->value);
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
							auto s = c->self;
							auto& v = s->value;
							auto& sab = v.aabb;
							if (sab.from.x < ab.to.x) {
								if (!s->flag) {
									s->flag = 1;
									results.Emplace(&v);
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
						auto s = c->self;
						auto& v = s->value;
						auto& sab = v.aabb;
						if (sab.to.x > ab.from.x && sab.from.y < ab.to.y) {
							if (!s->flag) {
								s->flag = 1;
								results.Emplace(&v);
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
							auto s = c->self;
							auto& v = s->value;
							auto& sab = v.aabb;
							if (sab.from.y < ab.to.y) {
								if (!s->flag) {
									s->flag = 1;
									results.Emplace(&v);
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
							auto s = c->self;
							auto& v = s->value;
							auto& sab = v.aabb;
							if (sab.from.x < ab.to.x && sab.from.y < ab.to.y) {
								if (!s->flag) {
									s->flag = 1;
									results.Emplace(&v);
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

/*
some examples


struct Foo {
	xx::FromTo<XY> aabb;

	void Init(XY const& pos, XY const& size) {
		auto s2 = size / 2;
		aabb.from = pos - s2;
		aabb.to = pos + s2;
	}

	void MoveBy(XY const& offset) {
		aabb.from += offset;
		aabb.to += offset;
	}
};



	xx::SpaceABGrid<Foo> sg;
	sg.Init(10, 10, 10);

	XY pos{ 5,5 }, siz{ 9, 9 };
	auto& f = sg.EmplaceInit(pos, siz);
	auto& o = xx::SpaceABNode<Foo>::From(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	sg.ForeachPoint({ 6,6 }, [](Foo& f) {
		xx::CoutN("found f from ", f.aabb.from, " to ", f.aabb.to);
		});

	xx::SpaceABWeak<Foo> w(f);
	xx::CoutN("w exists? ", !!w);

	f.MoveBy({7,7});
	sg.Update(f);
	xx::CoutN("from ", o.crIdx.from, " to ", o.crIdx.to);

	sg.ForeachPoint({ 13,13 }, [](Foo& f) {
		xx::CoutN("found f from ", f.aabb.from, " to ", f.aabb.to);
		});

	auto& f2 = sg.EmplaceInit(pos, siz);
	xx::CoutN("add f2");


	sg.ForeachPoint({ 8,8 }, [](Foo& f) {
		xx::CoutN("found f from ", f.aabb.from, " to ", f.aabb.to);
		});


	sg.ForeachAABB({ {0,0},{99,99} });
	if (!sg.results.Empty()) {
		for (auto& p : sg.results) {
			xx::CoutN("found f from ", p->aabb.from, " to ", p->aabb.to);
		}
	}
	sg.ClearResults();

	sg.Remove(f);
	xx::CoutN("w exists? ", !!w);

*/