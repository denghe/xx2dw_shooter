#pragma once
#include "xx_blocklist.h"

namespace xx {
	template<typename T>
	struct SpaceNode : VersionNextIndexTypeId {
		int32_t prev, cidx;
		T value;
	};

	// requires
	// T has member: static constexpr int32_t cTypeId 
	// T has member: float x, y
	template<typename T>
	struct Space : protected BlockList<T, SpaceNode> {
		using ST = BlockList<T, SpaceNode>;
		using ST::ST;

		int32_t numRows{}, numCols{}, cellSize{};
		int32_t maxX{}, maxY{};
		int32_t cellsLen{};
		std::unique_ptr<int32_t[]> cells;

		// Emplace + Init( args ) + store inti cells[ pos ]
		template<typename U = T, typename...Args>
		U& EmplaceInit(Args&&...args) {
			auto index = ST::Alloc();
			auto& o = RefNode(index);
			o.version = ST::GenVersion();
			o.next = -1;
			o.index = index;
			o.typeId = T::cTypeId;
			new (&o.value) U();
			o.value.Init(std::forward<Args>(args)...);

			auto cidx = PosToCIdx(o.value.x, o.value.y);
			auto head = cells[cidx];	// backup
			if (head >= 0) {
				ST::RefNode(head).prev = index;
			}
			cells[cidx] = index;			// assign new
			o.next = head;
			o.prev = -1;
			o.cidx = cidx;
			return (U&)o.value;
		}

		XX_FORCE_INLINE int32_t PosToCIdx(float x, float y) {
			assert(x >= 0 && x < cellSize * numCols);
			assert(y >= 0 && y < cellSize * numRows);
			auto c = int32_t(x) / cellSize;
			auto r = int32_t(y) / cellSize;
			return r * numCols + c;
		}

		void Remove(int32_t index) {
			auto& o = ST::RefNode(index);
			assert(o.version);
			assert(o.prev != o.index && o.next != o.index && o.version && o.cidx >= 0 && o.index == index);

			if (index == cells[o.cidx]) {
				cells[o.cidx] = o.next;
			}
			if (o.prev >= 0) {
				ST::RefNode(o.prev).next = o.next;
			}
			if (o.next >= 0) {
				ST::RefNode(o.next).prev = o.prev;
			}

			//o.typeId = -1;
			//o.index = -1;
			o.prev = -1;
			o.cidx = -1;
			o.version = 0;
			o.next = ST::freeHead;
			o.value.~T();
			ST::freeHead = index;
			++ST::freeCount;
			ST::FlagUnset(index);
		}

		void Remove(T const& v) {
			auto& o = *container_of(&v, SpaceNode, value);
			Remove(o.index);
		}

		void Update(T& v) {
			auto& o = *container_of(&v, SpaceNode, value);
			assert(o.index >= 0);
			assert(o.prev != o.index);
			assert(o.next != o.index);
			auto cidx = PosToCIdx(v.x, v.y);
			if (cidx == o.cidx) return;				// no change

			// unlink
			if (o.index != cells[o.cidx]) {			// isn't header
				ST::RefNode(o.prev).next = o.next;
				if (o.next >= 0) {
					ST::RefNode(o.next).prev = o.prev;
					//o.next = -1;
				}
				//o.prev = -1;
			} else {
				assert(o.prev == -1);
				cells[o.cidx] = o.next;
				if (o.next >= 0) {
					ST::RefNode(o.next).prev = -1;
					//o.next = -1;
				}
			}
			//o.cidx = -1;

			// relink
			if (cells[cidx] >= 0) {
				ST::RefNode(cells[cidx]).prev = o.index;
			}
			o.next = cells[cidx];
			o.prev = -1;
			cells[cidx] = o.index;
			o.cidx = cidx;
		}
	};

	template<typename T>
	struct SpaceWeak {
		using Node = SpaceNode<T>;

		T* pointer{};
		uint32_t version{};

		XX_FORCE_INLINE Node& RefNode() const {
			return (Node&)*container_of(pointer, Node, value);
		}

		XX_FORCE_INLINE operator bool() const noexcept {
			return pointer && version && version == RefNode().version;
		}

		XX_FORCE_INLINE T& operator()() const {
			assert(*this);
			return (T&)*pointer;
		}

		XX_FORCE_INLINE void Reset() {
			pointer = {};
			version = 0;
		}

		static SpaceWeak Make(T const& v) {
			auto& o = *container_of(&v, Node, value);
			return { (T*)&v, o.version };
		}
	};

}
