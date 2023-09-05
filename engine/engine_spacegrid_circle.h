﻿#pragma once
#include "engine_prims.h"

// space grid index system for circle
template<typename Item>
struct SpaceGridC;

// for inherit
template<typename Derived>
struct SpaceGridCItem {
    SpaceGridC<Derived>* _sgc{};
    Derived* _sgcPrev{}, * _sgcNext{};
    int32_t _sgcIdx{ -1 };
    Vec2<int32_t> _sgcPos;

    void SGCInit(SpaceGridC<Derived>* const& sgc) {
        assert(!_sgc);
        _sgc = sgc;
    }

    void SGCSetPos(Vec2<int32_t> const& pos) {
        assert(_sgc);
        assert(_sgcPos.x >= 0 && _sgcPos.x < _sgc->maxX);
        assert(_sgcPos.y >= 0 && _sgcPos.y < _sgc->maxY);
        _sgcPos = pos;
    }

    void SGCAdd() {
        assert(_sgc);
        _sgc->Add(((Derived*)(this)));
    }
    void SGCUpdate() {
        assert(_sgc);
        _sgc->Update(((Derived*)(this)));
    }
    void SGCRemove() {
        assert(_sgc);
        _sgc->Remove(((Derived*)(this)));
    }

    void SGCInit(SpaceGridC<Derived>* const& sgc, Vec2<int32_t> const& pos) {
        assert(!_sgc);
        _sgc = sgc;
        SGCSetPos(pos);
        SGCAdd();
    }
    void SGCUpdate(Vec2<int32_t> const& pos) {
        SGCSetPos(pos);
        SGCUpdate();
    }
    void SGCTryRemove() {
        if (_sgc) {
            SGCRemove();
            _sgc = {};
        }
    }
};

template<typename Item>
struct SpaceGridC {
    int32_t numRows{}, numCols{}, maxDiameter{};
    int32_t maxY{}, maxX{}, maxY1{}, maxX1{}, numItems{}, numActives{};	// for easy check & stat
    std::vector<Item*> cells;

    int32_t PosToIndex(Vec2<int32_t> const& p) {
        auto rcIdx = p / maxDiameter;
        return rcIdx.y * numCols + rcIdx.x;
    }

    void Init(int32_t const& numRows_, int32_t const& numCols_, int32_t const& maxDiameter_) {
        numRows = numRows_;
        numCols = numCols_;
        maxDiameter = maxDiameter_;
        maxY = maxDiameter * numRows;
        maxX = maxDiameter * numCols;
        maxY1 = maxY - 1;
        maxX1 = maxX - 1;
        cells.clear();
        cells.resize(numRows * numCols);
    }

    void Add(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx == -1);
        assert(!c->_sgcPrev);
        assert(!c->_sgcNext);
        assert(c->_sgcPos.x >= 0 && c->_sgcPos.x < maxX);
        assert(c->_sgcPos.y >= 0 && c->_sgcPos.y < maxY);

        // calc rIdx & cIdx
        int rIdx = c->_sgcPos.y / maxDiameter, cIdx = c->_sgcPos.x / maxDiameter;
        int idx = rIdx * numCols + cIdx;
        assert(idx <= cells.size());
        assert(!cells[idx] || !cells[idx]->_sgcPrev);

        // link
        if (cells[idx]) {
            cells[idx]->_sgcPrev = c;
        }
        c->_sgcNext = cells[idx];
        c->_sgcIdx = idx;
        cells[idx] = c;
        assert(!cells[idx]->_sgcPrev);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);

        // stat
        ++numItems;
    }

    void Remove(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(!c->_sgcPrev && cells[c->_sgcIdx] == c || c->_sgcPrev->_sgcNext == c && cells[c->_sgcIdx] != c);
        assert(!c->_sgcNext || c->_sgcNext->_sgcPrev == c);
        //assert(cells[c->_sgcIdx] include c);

        // unlink
        if (c->_sgcPrev) {	// isn't header
            assert(cells[c->_sgcIdx] != c);
            c->_sgcPrev->_sgcNext = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = c->_sgcPrev;
                c->_sgcNext = {};
            }
            c->_sgcPrev = {};
        } else {
            assert(cells[c->_sgcIdx] == c);
            cells[c->_sgcIdx] = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = {};
                c->_sgcNext = {};
            }
        }
        assert(cells[c->_sgcIdx] != c);
        c->_sgcIdx = -1;

        // stat
        --numItems;
    }

    void Update(Item* const& c) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx > -1);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);
        //assert(cells[c->_sgcIdx] include c);

        auto idx = PosToCellIdx(c->_sgcPos);
        if (idx == c->_sgcIdx) return;	// no change
        assert(!cells[idx] || !cells[idx]->_sgcPrev);
        assert(!cells[c->_sgcIdx] || !cells[c->_sgcIdx]->_sgcPrev);

        // unlink
        if (c->_sgcPrev) {	// isn't header
            assert(cells[c->_sgcIdx] != c);
            c->_sgcPrev->_sgcNext = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = c->_sgcPrev;
                //c->_sgcNext = {};
            }
            //c->_sgcPrev = {};
        } else {
            assert(cells[c->_sgcIdx] == c);
            cells[c->_sgcIdx] = c->_sgcNext;
            if (c->_sgcNext) {
                c->_sgcNext->_sgcPrev = {};
                //c->_sgcNext = {};
            }
        }
        //c->_sgcIdx = -1;
        assert(cells[c->_sgcIdx] != c);
        assert(idx != c->_sgcIdx);

        // link
        if (cells[idx]) {
            cells[idx]->_sgcPrev = c;
        }
        c->_sgcPrev = {};
        c->_sgcNext = cells[idx];
        cells[idx] = c;
        c->_sgcIdx = idx;
        assert(!cells[idx]->_sgcPrev);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);
    }

    // return x: col index   y: row index
    XX_FORCE_INLINE Vec2<int32_t> PosToCrIdx(Vec2<int32_t> const& pos) {
        assert(pos.x >= 0 && pos.x < maxX);
        assert(pos.y >= 0 && pos.y < maxY);
        return pos / maxDiameter;
    }

    // return cell's index
    XX_FORCE_INLINE int32_t CrIdxToCellIdx(Vec2<int32_t> const& crIdx) {
        return crIdx.y * numCols + crIdx.x;
    }

    // return cell's index
    XX_FORCE_INLINE int32_t PosToCellIdx(Vec2<int32_t> const& pos) {
        return CrIdxToCellIdx(PosToCrIdx(pos));
    }

    // find target cell. return true: break
    // F == [&](Item* o)->bool { ... return false; }
    template<typename F>
    bool Foreach(int32_t cellIndex, F&& f) {
        auto c = cells[cellIndex];
        while (c) {
            assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
            assert(c->_sgcNext != c);
            assert(c->_sgcPrev != c);
            if (f(c)) return true;
            c = c->_sgcNext;
        }
        return false;
    }

    constexpr static std::array<Vec2<int32_t>, 9> offsets9 = { 
        Vec2<int32_t>{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
    };

    // find target cell + round 8 = 9 cells. return true: break
    // F == [&](Item* o)->bool { ... return false; }
    template<typename F>
    void Foreach9(Vec2<int32_t> const& crIdx, F&& f) {
        for (auto& offset : offsets9) {
            auto cellIndex = CrIdxToCellIdx(crIdx + offset);
            if (cellIndex < 0 || cellIndex >= cells.size()) continue;
            auto c = cells[cellIndex];
            while (c) {
                assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
                assert(c->_sgcNext != c);
                assert(c->_sgcPrev != c);
                if (f(c)) return;
                c = c->_sgcNext;
            }
        }
    }

    // find target cell + ring diffuse cells. return true: break
    // F == [&](Item* o)->bool { ... return false; }
    template<typename F>
    void ForeachCells(Vec2<int32_t> const& crIdx, Vec2<int32_t> const* offsets, int size, F&& f) {
        for (int i = 0; i < size; ++i) {
            auto cellIndex = CrIdxToCellIdx(crIdx + offsets[i]);
            if (cellIndex < 0 || cellIndex >= cells.size()) continue;
            auto c = cells[cellIndex];
            while (c) {
                assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
                assert(c->_sgcNext != c);
                assert(c->_sgcPrev != c);
                if (f(c)) return;
                c = c->_sgcNext;
            }
        }
    }
};