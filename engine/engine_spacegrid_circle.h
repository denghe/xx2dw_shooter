#pragma once
#include <engine_spacegrid_ringdiffuse.h>

// space grid index system for circle
template<typename Item, typename XY_t = XYi>
struct SpaceGridC;

// for inherit
template<typename Derived, typename XY_t = XYi>
struct SpaceGridCItem {
    SpaceGridC<Derived, XY_t> *_sgc{};
    Derived *_sgcPrev{}, *_sgcNext{};
    int32_t _sgcIdx{ -1 };

    XX_FORCE_INLINE void SGCAdd(SpaceGridC<Derived, XY_t>& sgc, XY_t const& pos) {
        assert(!_sgc);
        _sgc = &sgc;
        _sgc->Add((Derived*)this, pos);
    }

    XX_FORCE_INLINE void SGCUpdate(XY_t const& pos) {
        assert(_sgc);
        _sgc->Update((Derived*)this, pos);
    }

    XX_FORCE_INLINE void SGCRemove() {
        if (_sgc) {
            _sgc->Remove((Derived*)this);
            _sgc = {};
        }
    }
};

template<typename Item, typename XY_t>
struct SpaceGridC {
    int32_t numRows{}, numCols{}, maxDiameter{};
    int32_t maxY{}, maxX{}, maxY1{}, maxX1{}, numItems{}, numActives{};	// for easy check & stat
    std::vector<Item*> cells;

    void Init(int32_t const& numRows_, int32_t numCols_, int32_t maxDiameter_) {
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

    void Add(Item* c, XY_t const& pos) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx == -1);
        assert(!c->_sgcPrev);
        assert(!c->_sgcNext);
        assert(pos.x >= 0 && pos.x < maxX);
        assert(pos.y >= 0 && pos.y < maxY);

        // calc rIdx & cIdx
        auto idx = PosToCellIdx(pos);
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

    void Remove(Item* c) {
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

    void Update(Item* c, XY_t const& pos) {
        assert(c);
        assert(c->_sgc == this);
        assert(c->_sgcIdx > -1);
        assert(c->_sgcNext != c);
        assert(c->_sgcPrev != c);
        //assert(cells[c->_sgcIdx] include c);

        auto idx = PosToCellIdx(pos);
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
    XX_FORCE_INLINE XYi PosToCrIdx(XY_t const& pos) {
        assert(pos.x >= 0 && pos.x < maxX);
        assert(pos.y >= 0 && pos.y < maxY);
        return { int32_t(pos.x / maxDiameter), int32_t(pos.y / maxDiameter) };
    }
    
    // return cell's index
    XX_FORCE_INLINE int32_t CrIdxToCellIdx(XYi const& crIdx) {
        return crIdx.y * numCols + crIdx.x;
    }
    
    // return cell's index
    XX_FORCE_INLINE int32_t PosToCellIdx(XY_t const& pos) {
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
            auto next = c->_sgcNext;
            if (f(c)) return true;
            c = next;
        }
        return false;
    }

    // F == [&](Item* o)->void { ... }
    template<typename F>
    void ForeachWithoutBreak(int32_t cellIndex, F&& f) {
        auto c = cells[cellIndex];
        while (c) {
            assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
            assert(c->_sgcNext != c);
            assert(c->_sgcPrev != c);
            auto next = c->_sgcNext;
            f(c);
            c = next;
        }
    }

    constexpr static std::array<XYi, 9> offsets9 = {
        XYi{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}
    };

    // find target cell + round 8 = 9 cells. return true: break
    // F == [&](Item* o)->bool { ... return false; }
    template<typename F>
    void Foreach9(XYi const& crIdx, F&& f) {
        for (auto& offset : offsets9) {
            auto cellIndex = CrIdxToCellIdx(crIdx + offset);
            if (cellIndex < 0 || cellIndex >= cells.size()) continue;   // todo: fix
            auto c = cells[cellIndex];
            while (c) {
                assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
                assert(c->_sgcNext != c);
                assert(c->_sgcPrev != c);
                auto next = c->_sgcNext;
                if (f(c)) return;
                c = next;
            }
        }
    }

    // find target cell + ring diffuse cells. return true: break
    // F == [&](Item* o)->bool { ... return false; }
    template<typename F>
    void ForeachCells(XYi const& crIdx, XYi const* offsets, int size, F&& f) {
        for (int i = 0; i < size; ++i) {
            auto cellIndex = CrIdxToCellIdx(crIdx + offsets[i]);
            if (cellIndex < 0 || cellIndex >= cells.size()) continue;   // todo: fix
            auto c = cells[cellIndex];
            while (c) {
                assert(cells[c->_sgcIdx]->_sgcPrev == nullptr);
                assert(c->_sgcNext != c);
                assert(c->_sgcPrev != c);
                auto next = c->_sgcNext;
                if (f(c)) return;
                c = next;
            }
        }
    }

};



template<typename T>
T* FindNeighborCross(SpaceGridC<T, XY>& container, XY const& pos, float radius) {
    assert(radius * 2 <= container.maxDiameter);
    auto crIdx = container.PosToCrIdx(pos);
    T* r{};
    container.Foreach9(crIdx, [&](T* m)->bool {
        // (r1 + r2) * (r1 + r2) > (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)
        auto d = m->pos - pos;
        auto rr = (m->radius + radius) * (m->radius + radius);
        auto dd = d.x * d.x + d.y * d.y;
        if (rr > dd) {
            r = m;
            return true;
        }
        return false;
    });
    return r;
}

template<typename T, typename F>
void FindNeighborsCross(SpaceGridC<T, XY>& container, XY const& pos, float radius, F&& func) {
    assert(radius * 2 <= container.maxDiameter);
    auto crIdx = container.PosToCrIdx(pos);
    container.Foreach9(crIdx, [&](T* m)->bool {
        auto d = m->pos - pos;
        auto rr = (m->radius + radius) * (m->radius + radius);
        auto dd = d.x * d.x + d.y * d.y;
        if (rr > dd) {
            func(m);
        }
        return false;
    });
}


template<typename T>
T* FindNearest(SpaceGridC<T, XY>& container, SpaceGridRingDiffuseData const& sgrdd, XY const& pos, float maxDistance) {
    auto crIdx = container.PosToCrIdx(pos);					// calc grid col row index

    float minVxxyy = maxDistance * maxDistance;
    T* o{};
    XY ov;

    auto& lens = sgrdd.lens;
    auto& idxs = sgrdd.idxs;
    for (int i = 1; i < lens.len; i++) {

        auto offsets = &idxs[lens[i - 1].count];
        auto size = lens[i].count - lens[i - 1].count;
        container.ForeachCells(crIdx, offsets, size, [&](T* m)->bool {
            auto v = m->pos - pos;
            if (auto xxyy = v.x * v.x + v.y * v.y; xxyy < minVxxyy) {
                minVxxyy = xxyy;
                o = m;
                ov = v;
            }
            return false;
        });

        if (o) return o;									// found. stop ring diffuse step
        if (lens[i].radius > maxDistance) break;			// limit search range
    }
    return nullptr;
}

template<typename T, typename F>
void ForeachByRange(SpaceGridC<T, XY>& container, SpaceGridRingDiffuseData const& sgrdd, XY const& pos, float maxDistance, F&& func) {
    auto crIdx = container.PosToCrIdx(pos);					// calc grid col row index
    float rr = maxDistance * maxDistance;
    auto& lens = sgrdd.lens;
    auto& idxs = sgrdd.idxs;
    for (int i = 1; i < lens.len; i++) {

        auto offsets = &idxs[lens[i - 1].count];
        auto size = lens[i].count - lens[i - 1].count;
        container.ForeachCells(crIdx, offsets, size, [&](T* m)->bool {
            auto v = m->pos - pos;
            if (v.x * v.x + v.y * v.y < rr) {
                func(m);
            }
            return false;
        });

        if (lens[i].radius > maxDistance) break;			// limit search range
    }
}
