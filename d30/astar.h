#pragma once
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>
#include <cassert>
#include <algorithm>
#include <optional>
#include <chrono>

// 此代码不能直接用。主用于复制小改
namespace xx::AStar {
	struct Cell {
		int x = 0;
		int y = 0;
		int walkable = 0;
		Cell* parent = nullptr;

		float heuristicStartToEndLen = 0.0f;
		float startToCurLen = 0.0f;
		float heuristicCurToEndLen = 0.0f;
		char heuristicCurToEndLen_hasValue = 0;
		char opened = 0;
		char closed = 0;

		inline void Clear() {
			memset(&heuristicStartToEndLen, 0, 4 * 4);
		}

		inline void Fill(std::vector<Cell*>& path) {
			path.clear();
			auto c = this;
			path.push_back(this);
			while ((c = c->parent)) {
				path.push_back(c);
			}
		}
	};

	struct CellHeap {
		struct Comparer {
			bool operator() (Cell const* x, Cell const* y) const noexcept {
				return x->heuristicStartToEndLen > y->heuristicStartToEndLen;
			}
		} comparer;

		std::vector<Cell*> data;

		void Clear() {
			data.clear();
		}

		bool Empty() const {
			return data.empty();
		}

		void Add(Cell* const& c) {
			data.emplace_back(c);
			std::push_heap(data.begin(), data.end(), comparer);
		}

		Cell* DeleteMin() {
			auto from = data.front();
			std::pop_heap(data.begin(), data.end(), comparer);
			data.pop_back();
			return from;
		}
	};

	struct Grid {
		int width = 0;
		int height = 0;
		Cell* startCell = nullptr;
		Cell* endCell = nullptr;
		std::vector<Cell> cells;
		std::vector<Cell*> path;
		static constexpr std::array<std::pair<int, int>, 8> neighborOffsets = {
			std::pair<int, int>{-1, -1}
			, std::pair<int, int>{0, -1}
			, std::pair<int, int>{1, -1}
			, std::pair<int, int>{-1, 0}
			, std::pair<int, int>{1, 0}
			, std::pair<int, int>{-1, 1}
			, std::pair<int, int>{0, 1}
			, std::pair<int, int>{1, 1}
		};
		const float sqrt_2 = sqrtf(2);
		CellHeap openList;

		Cell& At(int const& x, int const& y) {
			assert(x >= 0 && y >= 0 && x < width && y < height);
			return cells[(size_t)y * width + x];
		}

		float Heuristic(int const& x, int const& y) {
			return sqrtf(float((x - endCell->x) * (x - endCell->x) + (y - endCell->y) * (y - endCell->y)));
		}

		bool FindPath() {
			if (!startCell || !endCell) return false;
			Reset();
			openList.Clear();
			openList.Add(startCell);
			startCell->opened = 1;

			while (!openList.Empty()) {
				auto cell = openList.DeleteMin();
				cell->opened = 0;
				cell->closed = 1;

				if (cell == endCell) {
					cell->Fill(path);
					return true;
				}

				auto cx = cell->x;
				auto cy = cell->y;
				for (auto&& ni : neighborOffsets) {
					auto&& n = At(ni.first + cx, ni.second + cy);
					if (!n.walkable || n.closed/* && n.startToCurLen <= ng*/) continue;
					auto ng = cell->startToCurLen + ((n.x == cx || n.y == cy) ? 1.0f : sqrt_2);
					if (!n.opened || ng < n.startToCurLen) {
						n.startToCurLen = ng;
						if (!n.heuristicCurToEndLen_hasValue) {
							n.heuristicCurToEndLen = Heuristic(n.x, n.y);
							n.heuristicCurToEndLen_hasValue = 1;
						}
						n.heuristicStartToEndLen = n.startToCurLen + n.heuristicCurToEndLen;
						n.parent = cell;
						if (!n.opened) {
							openList.Add(&n);
							n.opened = 1;
						}
					}
				}
			}
			return false;
		}

		void Reset() {
			for (auto&& c : cells) {
				c.Clear();
			}
		}

		void LoadByFile(char const* const& fileName) {
			std::ifstream f(fileName);
			std::string tmp;
			std::vector<std::string> ss;
			while (getline(f, tmp)) {
				if (tmp[tmp.size() - 1] < 36) {
					tmp.resize(tmp.size() - 1);
				}
				ss.push_back(tmp);
			}

			width = (int)ss[0].size();
			height = (int)ss.size();
			cells.resize((size_t)width * height);
			path.clear();
			startCell = nullptr;
			endCell = nullptr;

			for (int y = 0; y < height; ++y) {
				auto& s = ss[y];
				for (int x = 0; x < width; ++x) {
					switch (s[x]) {
					case '@':
						startCell = &At(x, y);
						At(x, y).walkable = 1;
						break;
					case '*':
						endCell = &At(x, y);
					case ' ':
						At(x, y).walkable = 1;
						break;
					default:
						At(x, y).walkable = 0;
					}
				}
			}

			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					At(x, y).x = x;
					At(x, y).y = y;
				}
			}
		}

		void Dump() {
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					auto o = &At(x, y);
					if (o == startCell) {
						std::cout << "o";
					}
					else if (o == endCell) {
						std::cout << "*";
					}
					else if (path.size() && std::find(path.begin(), path.end(), o) != path.end()) {
						std::cout << "+";
					}
					else if (o->walkable) {
						std::cout << " ";
					}
					else {
						std::cout << "#";
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	};

}
