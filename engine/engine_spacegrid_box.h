#pragma once
#include "engine_prims.h"

// space grid index system for AABB bounding box. coordinate (0, 0) at left-top, +x = right, +y = buttom
template<typename Item, typename XY_t = Vec2<int32_t>>
struct SpaceGridAB;

template<typename T>
struct SpaceGridABItemCellInfo {
	T* self{};
	size_t idx{};
	SpaceGridABItemCellInfo* prev{}, * next{};
};

// for inherit
template<typename Derived, typename XY_t = Vec2<int32_t>>
struct SpaceGridABItem {
	using SGABCoveredCellInfo = SpaceGridABItemCellInfo<Derived>;
	SpaceGridAB<Derived, XY_t>* _sgab{};
	XY_t _sgabPos, _sgabRadius, _sgabMin, _sgabMax;	// for Add & Update calc covered cells
	Vec2<int32_t> _sgabCRIdxFrom, _sgabCRIdxTo;	// backup for Update speed up
	std::vector<SGABCoveredCellInfo> _sgabCoveredCellInfos;	// todo: change to custom single buf container ?
	size_t _sgabFlag{};	// avoid duplication when Foreach

	XX_FORCE_INLINE void SGABSetPosSiz(XY_t const& pos, XY_t const& siz) {
		_sgabPos = pos;
		_sgabRadius = siz / 2;
		_sgabMin = pos - _sgabRadius;
		_sgabMax = pos + _sgabRadius;
	}

	XX_FORCE_INLINE void SGABAdd(SpaceGridAB<Derived, XY_t>& sgab, XY_t const& pos, XY_t const& siz) {
		assert(!_sgab);
		assert(!_sgabFlag);
		assert(_sgabCoveredCellInfos.empty());
		_sgab = &sgab;
		SGABSetPosSiz(pos, siz);
		_sgab->Add(((Derived*)(this)));
	}

	XX_FORCE_INLINE void SGABUpdate(XY_t const& pos, XY_t const& siz) {
		assert(_sgab);
		SGABSetPosSiz(pos, siz);
		_sgab->Update(((Derived*)(this)));
	}

	XX_FORCE_INLINE void SGABAddOrUpdate(SpaceGridAB<Derived, XY_t>& sgab, XY_t const& pos, XY_t const& siz) {
		if (_sgab) {
			assert(_sgab == &sgab);
			SGABUpdate(pos, siz);
		} else {
			SGABAdd(sgab, pos, siz);
		}
	}

	XX_FORCE_INLINE void SGABRemove() {
		if (_sgab) {
			_sgab->Remove(((Derived*)(this)));
			_sgab = {};
		}
	}
};

template<typename Item, typename XY_t>
struct SpaceGridAB {
	using ItemCellInfo = SpaceGridABItemCellInfo<Item>;
	Vec2<int32_t> cellSize;
	XY_t max, max_2;
	int32_t numRows{}, numCols{};
	int32_t numItems{}, numActives{};	// for easy check & stat
	std::vector<ItemCellInfo*> cells;
	std::vector<Item*> results;	// tmp store Foreach items

	void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& cellWidth_, int32_t const& cellHeight_) {
		assert(cells.empty());
		assert(!numItems);
		assert(!numActives);

		numRows = numRows_;
		numCols = numCols_;
		cellSize.x = cellWidth_;
		cellSize.y = cellHeight_;

		max.y = cellHeight_ * numRows;
		max.x = cellWidth_ * numCols;
		max_2 = max / 2;

		cells.resize(numRows * numCols);
	}

	void Add(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.empty());
		assert(c->_sgabMin.x < c->_sgabMax.x);
		assert(c->_sgabMin.y < c->_sgabMax.y);
		assert(c->_sgabMin.x >= 0 && c->_sgabMin.y >= 0);
		assert(c->_sgabMax.x < c->_sgab->max.x && c->_sgabMax.y < c->_sgab->max.y);

		// calc covered cells
		auto crIdxFrom = c->_sgabMin.template As<int32_t>() / cellSize;
		auto crIdxTo = c->_sgabMax.template As<int32_t>() / cellSize;
		auto numCoveredCells = (crIdxTo.x - crIdxFrom.x + 1) * (crIdxTo.y - crIdxFrom.y + 1);

		// link
		auto& ccis = c->_sgabCoveredCellInfos;
		ccis.reserve(numCoveredCells);
		for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
			for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
				size_t idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				auto ci = &ccis.emplace_back(ItemCellInfo{ c, idx, nullptr, cells[idx] });
				if (cells[idx]) {
					cells[idx]->prev = ci;
				}
				cells[idx] = ci;
			}
		}

		// store idxs
		c->_sgabCRIdxFrom = crIdxFrom;
		c->_sgabCRIdxTo = crIdxTo;

		// stat
		++numItems;
	}

	void Remove(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.size());

		// unlink
		auto& ccis = c->_sgabCoveredCellInfos;
		for (auto& ci : ccis) {

			if (ci.prev) {	// isn't header
				ci.prev->next = ci.next;
				if (ci.next) {
					ci.next->prev = ci.prev;
				}
			} else {
				cells[ci.idx] = ci.next;
				if (ci.next) {
					ci.next->prev = {};
				}
			}
		}
		ccis.clear();

		// stat
		--numItems;
	}

	void Update(Item* const& c) {
		assert(c);
		assert(c->_sgab == this);
		assert(c->_sgabCoveredCellInfos.size());
		assert(c->_sgabMin.x < c->_sgabMax.x);
		assert(c->_sgabMin.y < c->_sgabMax.y);
		assert(c->_sgabMin.x >= 0 && c->_sgabMin.y >= 0);
		assert(c->_sgabMax.x < c->_sgab->max.x && c->_sgabMax.y < c->_sgab->max.y);

		// calc covered cells
		auto crIdxFrom = c->_sgabMin.template As<int32_t>() / cellSize;
		auto crIdxTo = c->_sgabMax.template As<int32_t>() / cellSize;
		auto numCoveredCells = (crIdxTo.x - crIdxFrom.x + 1) * (crIdxTo.y - crIdxFrom.y + 1);

		auto& ccis = c->_sgabCoveredCellInfos;
		if (numCoveredCells == ccis.size()
			&& crIdxFrom == c->_sgabCRIdxFrom
			&& crIdxTo == c->_sgabCRIdxTo) {
			return;
		}

		// unlink
		for (auto& ci : ccis) {

			if (ci.prev) {	// isn't header
				ci.prev->next = ci.next;
				if (ci.next) {
					ci.next->prev = ci.prev;
				}
			} else {
				cells[ci.idx] = ci.next;
				if (ci.next) {
					ci.next->prev = {};
				}
			}
		}
		ccis.clear();

		// link
		ccis.reserve(numCoveredCells);
		for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
			for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
				size_t idx = rIdx * numCols + cIdx;
				assert(idx <= cells.size());
				auto ci = &ccis.emplace_back(ItemCellInfo{ c, idx, nullptr, cells[idx] });
				if (cells[idx]) {
					cells[idx]->prev = ci;
				}
				cells[idx] = ci;
			}
		}

		// store idxs
		c->_sgabCRIdxFrom = crIdxFrom;
		c->_sgabCRIdxTo = crIdxTo;
	}

	void ClearResults() {
		for (auto& o : results) {
			o->_sgabFlag = 0;
		}
		results.clear();
	}

	template<typename F>
	void ForeachPoint(XY_t const& p, F&& func) {
		auto crIdx = p.template As<int32_t>() / cellSize;
		if (crIdx.x < 0 || crIdx.x >= numCols
		 || crIdx.y < 0 || crIdx.y >= numRows) return;
		auto c = cells[crIdx.y * numCols + crIdx.x];
		while (c) {
			auto&& s = c->self;
			auto next = c->next;
			if (Calc::Intersects::BoxPoint(s->_sgabMin, s->_sgabMax, p)) {
				func(s);
			}
			c = next;
		}
	}

	// fill items to results. need ClearResults()
	// auto guard = xx::MakeSimpleScopeGuard([&] { sg.ClearResults(); });
	template<bool enableLimit = false, bool enableExcept = false>
	void ForeachAABB(XY_t const& minXY, XY_t const& maxXY, int32_t* limit = nullptr, Item* const& except = nullptr) {
		assert(minXY.x < maxXY.x);
		assert(minXY.y < maxXY.y);
		assert(minXY.x >= 0 && minXY.y >= 0);
		assert(maxXY.x < max.x && maxXY.y < max.y);

		// calc covered cells
		auto crIdxFrom = minXY.template As<int32_t>() / cellSize;
		auto crIdxTo = maxXY.template As<int32_t>() / cellSize;

		// except set flag
		if constexpr (enableExcept) {
			assert(except);
			except->_sgabFlag = 1;
		}

		if (crIdxFrom.x == crIdxTo.x || crIdxFrom.y == crIdxTo.y) {
			// 1-2 row, 1-2 col: do full rect check
			for (auto rIdx = crIdxFrom.y; rIdx <= crIdxTo.y; rIdx++) {
				for (auto cIdx = crIdxFrom.x; cIdx <= crIdxTo.x; cIdx++) {
					auto c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (Calc::Intersects::BoxBox(s->_sgabMin, s->_sgabMax, minXY, maxXY)) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
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
			auto rIdx = crIdxFrom.y;

			// first cell: check top left AB
			auto cIdx = crIdxFrom.x;
			auto c = cells[rIdx * numCols + cIdx];
			while (c) {
				auto&& s = c->self;
				if (s->_sgabMax.x > minXY.x && s->_sgabMax.y > minXY.y) {
					if (!s->_sgabFlag) {
						s->_sgabFlag = 1;
						results.push_back(s);
					}
					if constexpr (enableLimit) {
						if (--*limit == 0) break;
					}
				}
				c = c->next;
			}

			// middle cells: check top AB
			for (++cIdx; cIdx < crIdxTo.x; cIdx++) {
				c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					if (s->_sgabMax.y > minXY.y) {
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
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
			if (cIdx == crIdxTo.x) {
				auto c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					if (s->_sgabMin.x < maxXY.x && s->_sgabMax.y > minXY.y) {
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
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
			for (++rIdx; rIdx < crIdxTo.y; rIdx++) {

				// first cell: check left AB
				cIdx = crIdxFrom.x;
				c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					if (s->_sgabMax.x > minXY.x) {
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
							results.push_back(s);
						}
						if constexpr (enableLimit) {
							if (--*limit == 0) break;
						}
					}
					c = c->next;
				}

				// middle cols: no check
				for (; cIdx < crIdxTo.x; cIdx++) {
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
							results.push_back(s);
						}
						if constexpr (enableLimit) {
							if (--*limit == 0) break;
						}
						c = c->next;
					}
				}

				// last cell: check right AB
				if (cIdx == crIdxTo.x) {
					auto c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMin.x < maxXY.x) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
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
			if (rIdx == crIdxTo.y) {

				// first cell: check left bottom AB
				cIdx = crIdxFrom.x;
				c = cells[rIdx * numCols + cIdx];
				while (c) {
					auto&& s = c->self;
					if (s->_sgabMax.x > minXY.x && s->_sgabMin.y < maxXY.y) {
						if (!s->_sgabFlag) {
							s->_sgabFlag = 1;
							results.push_back(s);
						}
						if constexpr (enableLimit) {
							if (--*limit == 0) break;
						}
					}
					c = c->next;
				}

				// middle cells: check bottom AB
				for (++cIdx; cIdx < crIdxTo.x; cIdx++) {
					c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMin.y < maxXY.y) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
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
				if (cIdx == crIdxTo.x) {
					auto c = cells[rIdx * numCols + cIdx];
					while (c) {
						auto&& s = c->self;
						if (s->_sgabMin.x < maxXY.x && s->_sgabMin.y < maxXY.y) {
							if (!s->_sgabFlag) {
								s->_sgabFlag = 1;
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
			except->_sgabFlag = 0;
		}

	}
};
