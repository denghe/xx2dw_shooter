// https://github.com/Relintai/texture_packer/tree/master/rectpack2D

#pragma once
#include <vector>
#include <algorithm>
#include <cstring>

namespace rect_pack_2d {
	struct rect_ltrb;
	struct rect_xywh;

	struct rect_wh {
		rect_wh(const rect_ltrb&);
		rect_wh(const rect_xywh&);
		rect_wh(int w = 0, int h = 0);
		int w, h, area(), perimeter(),
			fits(const rect_wh& bigger, bool allowFlip) const; // 0 - no, 1 - yes, 2 - flipped, 3 - perfectly, 4 perfectly flipped
	};

	// rectangle implementing left/top/right/bottom behaviour

	struct rect_ltrb {
		rect_ltrb();
		rect_ltrb(int left, int top, int right, int bottom);
		int l, t, r, b, w() const, h() const, area() const, perimeter() const;
		void w(int), h(int);
	};

	struct rect_xywh : public rect_wh {
		rect_xywh();
		rect_xywh(const rect_ltrb&);
		rect_xywh(int x, int y, int width, int height);
		operator rect_ltrb();

		int x, y, r() const, b() const;
		void r(int), b(int);
	};

	struct rect_xywhf : public rect_xywh {
		rect_xywhf(const rect_ltrb&);
		rect_xywhf(int x, int y, int width, int height);
		rect_xywhf();
		void flip();
		bool flipped;

		int refcount;

		void* ud{};
	};

	struct bin {
		rect_wh size;
		std::vector<rect_xywhf*> rects;
	};

	bool pack(rect_xywhf* const* v, int n, int max_side, bool allowFlip, std::vector<bin>& bins);

	using namespace std;

	inline bool area(rect_xywhf* a, rect_xywhf* b) {
		return a->area() > b->area();
	}

	inline bool perimeter(rect_xywhf* a, rect_xywhf* b) {
		return a->perimeter() > b->perimeter();
	}

	inline bool max_side(rect_xywhf* a, rect_xywhf* b) {
		return std::max(a->w, a->h) > std::max(b->w, b->h);
	}

	inline bool max_width(rect_xywhf* a, rect_xywhf* b) {
		return a->w > b->w;
	}

	inline bool max_height(rect_xywhf* a, rect_xywhf* b) {
		return a->h > b->h;
	}

	// just add another comparing function name to cmpf to perform another packing attempt
	// more functions == slower but probably more efficient cases covered and hence less area wasted

	constexpr bool (*cmpf[])(rect_xywhf*, rect_xywhf*) = {
		area,
		perimeter,
		max_side,
		max_width,
		max_height
	};

	// if you find the algorithm running too slow you may double this factor to increase speed but also decrease efficiency
	// 1 == most efficient, slowest
	// efficiency may be still satisfying at 64 or even 256 with nice speedup

	constexpr int discard_step = 128;

	/*

	For every sorting function, algorithm will perform packing attempts beginning with a bin with width and height equal to max_side,
	and decreasing its dimensions if it finds out that rectangles did actually fit, increasing otherwise.
	Although, it's doing that in sort of binary search manner, so for every comparing function it will perform at most log2(max_side) packing attempts looking for the smallest possible bin size.
	discard_step = 128 means that the algorithm will break of the searching loop if the rectangles fit but "it may be possible to fit them in a bin smaller by 128"
	the bigger the value, the sooner the algorithm will finish but the rectangles will be packed less tightly.
	use discard_step = 1 for maximum tightness.

	the algorithm was based on http://www.blackpawn.com/texts/lightmaps/default.html
	the algorithm reuses the node tree so it doesn't reallocate them between searching attempts

	*/

	/*************************************************************************** CHAOS BEGINS HERE */

	struct node {
		struct pnode {
			node* pn = nullptr;
			bool fill = false;

			void set(int l, int t, int r, int b) {
				if (!pn)
					pn = new node(rect_ltrb(l, t, r, b));
				else {
					(*pn).rc = rect_ltrb(l, t, r, b);
					(*pn).id = false;
				}
				fill = true;
			}
		};

		pnode c[2];
		rect_ltrb rc;
		bool id = false;
		node(rect_ltrb rect_rc = rect_ltrb()) :
			rc(rect_rc) {}

		void reset(const rect_wh& r) {
			id = false;
			rc = rect_ltrb(0, 0, r.w, r.h);
			delcheck();
		}

		node* insert(rect_xywhf& img, bool allowFlip) {
			if (c[0].pn && c[0].fill) {
				if (auto newn = c[0].pn->insert(img, allowFlip)) return newn;
				return c[1].pn->insert(img, allowFlip);
			}

			if (id) return 0;
			int f = img.fits(rect_xywh(rc), allowFlip);

			switch (f) {
			case 0: return 0;
			case 1: img.flipped = false; break;
			case 2: img.flipped = true; break;
			case 3:
				id = true;
				img.flipped = false;
				return this;
			case 4:
				id = true;
				img.flipped = true;
				return this;
			}

			int iw = (img.flipped ? img.h : img.w), ih = (img.flipped ? img.w : img.h);

			if (rc.w() - iw > rc.h() - ih) {
				c[0].set(rc.l, rc.t, rc.l + iw, rc.b);
				c[1].set(rc.l + iw, rc.t, rc.r, rc.b);
			} else {
				c[0].set(rc.l, rc.t, rc.r, rc.t + ih);
				c[1].set(rc.l, rc.t + ih, rc.r, rc.b);
			}

			return c[0].pn->insert(img, allowFlip);
		}

		void delcheck() {
			if (c[0].pn) {
				c[0].fill = false;
				c[0].pn->delcheck();
			}
			if (c[1].pn) {
				c[1].fill = false;
				c[1].pn->delcheck();
			}
		}

		~node() {
			if (c[0].pn) delete c[0].pn;
			if (c[1].pn) delete c[1].pn;
		}
	};

	inline rect_wh _rect2D(rect_xywhf* const* v, int n, int max_s, bool allowFlip, vector<rect_xywhf*>& succ, vector<rect_xywhf*>& unsucc) {
		node root;

		const int funcs = (sizeof(cmpf) / sizeof(bool (*)(rect_xywhf*, rect_xywhf*)));

		rect_xywhf** order[funcs];

		for (int f = 0; f < funcs; ++f) {
			order[f] = new rect_xywhf * [n];
			std::memcpy(order[f], v, sizeof(rect_xywhf*) * n);
			sort(order[f], order[f] + n, cmpf[f]);
		}

		rect_wh min_bin = rect_wh(max_s, max_s);
		int min_func = -1, best_func = 0, best_area = 0, _area = 0, step, fit, i;

		bool fail = false;

		for (int f = 0; f < funcs; ++f) {
			v = order[f];
			step = min_bin.w / 2;
			root.reset(min_bin);

			while (true) {
				if (root.rc.w() > min_bin.w) {
					if (min_func > -1) break;
					_area = 0;

					root.reset(min_bin);
					for (i = 0; i < n; ++i)
						if (root.insert(*v[i], allowFlip))
							_area += v[i]->area();

					fail = true;
					break;
				}

				fit = -1;

				for (i = 0; i < n; ++i)
					if (!root.insert(*v[i], allowFlip)) {
						fit = 1;
						break;
					}

				if (fit == -1 && step <= discard_step)
					break;

				root.reset(rect_wh(root.rc.w() + fit * step, root.rc.h() + fit * step));

				step /= 2;
				if (!step)
					step = 1;
			}

			if (!fail && (min_bin.area() >= root.rc.area())) {
				min_bin = rect_wh(root.rc);
				min_func = f;
			}

			else if (fail && (_area > best_area)) {
				best_area = _area;
				best_func = f;
			}
			fail = false;
		}

		v = order[min_func == -1 ? best_func : min_func];

		int clip_x = 0, clip_y = 0;

		root.reset(min_bin);

		for (i = 0; i < n; ++i) {
			if (auto ret = root.insert(*v[i], allowFlip)) {
				v[i]->x = ret->rc.l;
				v[i]->y = ret->rc.t;

				if (v[i]->flipped) {
					v[i]->flipped = false;
					v[i]->flip();
				}

				clip_x = std::max(clip_x, ret->rc.r);
				clip_y = std::max(clip_y, ret->rc.b);

				succ.push_back(v[i]);
			} else {
				unsucc.push_back(v[i]);

				v[i]->flipped = false;
			}
		}

		for (int f = 0; f < funcs; ++f)
			delete[] order[f];

		return rect_wh(clip_x, clip_y);
	}

	inline bool pack(rect_xywhf* const* v, int n, int max_s, bool allowFlip, vector<bin>& bins) {
		rect_wh _rect(max_s, max_s);

		for (int i = 0; i < n; ++i)
			if (!v[i]->fits(_rect, allowFlip)) return false;

		vector<rect_xywhf*> vec[2], * p[2] = { vec, vec + 1 };
		vec[0].resize(n);
		vec[1].clear();
		std::memcpy(&vec[0][0], v, sizeof(rect_xywhf*) * n);

		while (true) {
			auto& b = bins.emplace_back();

			b.size = _rect2D(&((*p[0])[0]), static_cast<int>(p[0]->size()), max_s, allowFlip, b.rects, *p[1]);
			p[0]->clear();

			if (!p[1]->size()) break;

			std::swap(p[0], p[1]);
		}

		return true;
	}

	inline rect_wh::rect_wh(const rect_ltrb& rr) :
		w(rr.w()),
		h(rr.h()) {}
	inline rect_wh::rect_wh(const rect_xywh& rr) :
		w(rr.w),
		h(rr.h) {}
	inline rect_wh::rect_wh(int p_w, int p_h) :
		w(p_w),
		h(p_h) {}

	inline int rect_wh::fits(const rect_wh& p_r, bool p_allow_flip) const {
		if (w == p_r.w && h == p_r.h) return 3;
		if (p_allow_flip && h == p_r.w && w == p_r.h) return 4;
		if (w <= p_r.w && h <= p_r.h) return 1;
		if (p_allow_flip && h <= p_r.w && w <= p_r.h) return 2;
		return 0;
	}

	inline rect_ltrb::rect_ltrb() :
		l(0),
		t(0),
		r(0),
		b(0) {}
	inline rect_ltrb::rect_ltrb(int p_l, int p_t, int p_r, int p_b) :
		l(p_l),
		t(p_t),
		r(p_r),
		b(p_b) {}

	inline int rect_ltrb::w() const {
		return r - l;
	}

	inline int rect_ltrb::h() const {
		return b - t;
	}

	inline int rect_ltrb::area() const {
		return w() * h();
	}

	inline int rect_ltrb::perimeter() const {
		return 2 * w() + 2 * h();
	}

	inline void rect_ltrb::w(int ww) {
		r = l + ww;
	}

	inline void rect_ltrb::h(int hh) {
		b = t + hh;
	}

	inline rect_xywh::rect_xywh() :
		x(0),
		y(0) {}
	inline rect_xywh::rect_xywh(const rect_ltrb& p_rc) :
		x(p_rc.l),
		y(p_rc.t) {
		b(p_rc.b);
		r(p_rc.r);
	}
	inline rect_xywh::rect_xywh(int p_x, int p_y, int p_w, int p_h) :
		rect_wh(p_w, p_h),
		x(p_x),
		y(p_y) {}

	inline rect_xywh::operator rect_ltrb() {
		rect_ltrb rr(x, y, 0, 0);
		rr.w(w);
		rr.h(h);
		return rr;
	}

	inline int rect_xywh::r() const {
		return x + w;
	};

	inline int rect_xywh::b() const {
		return y + h;
	}

	inline void rect_xywh::r(int p_right) {
		w = p_right - x;
	}

	inline void rect_xywh::b(int p_bottom) {
		h = p_bottom - y;
	}

	inline int rect_wh::area() {
		return w * h;
	}

	inline int rect_wh::perimeter() {
		return 2 * w + 2 * h;
	}

	inline rect_xywhf::rect_xywhf(const rect_ltrb& p_rr) :
		rect_xywh(p_rr),
		flipped(false) {}
	inline rect_xywhf::rect_xywhf(int p_x, int p_y, int p_width, int p_height) :
		rect_xywh(p_x, p_y, p_width, p_height),
		flipped(false) {}
	inline rect_xywhf::rect_xywhf() :
		flipped(false) {}

	inline void rect_xywhf::flip() {
		flipped = !flipped;
		std::swap(w, h);
	}
}
