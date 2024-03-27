#include "pch.h"
#include "looper.h"
#include "scene_main_menu.h"
#include "scene_test1.h"
#include "xx_blocklink.h"

namespace xx {

	template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
	struct FixedListPool;

	// copy code fom List ( maybe combine with List ? )
	template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
	struct FixedListCore {
		typedef T ChildType;
		using Pool = FixedListPool<T, SizeType, cap>;

		Pool* pool;
		SizeType len;
		T buf[0];

		FixedListCore() = delete;
		FixedListCore(FixedListCore const& o) = delete;
		FixedListCore& operator=(FixedListCore const& o) = delete;
		~FixedListCore() noexcept {
			Clear();
		}

		template<typename...Args>
		T& Emplace(Args&&...args) {
			assert(len < cap);
			return *new (&buf[len++]) T(std::forward<Args>(args)...);
		}

		template<typename ...TS>
		void Add(TS&&...vs) {
			(Emplace(std::forward<TS>(vs)), ...);
		}

		void RemoveAt(SizeType idx) {
			assert(idx >= 0 && idx < len);
			--len;
			if constexpr (IsPod_v<T>) {
				buf[idx].~T();
				::memmove(buf + idx, buf + idx + 1, (len - idx) * sizeof(T));
			} else {
				for (SizeType i = idx; i < len; ++i) {
					buf[i] = (T&&)buf[i + 1];
				}
				buf[len].~T();
			}
		}

		void SwapRemoveAt(SizeType idx) {
			assert(idx >= 0 && idx < len);
			buf[idx].~T();
			--len;
			if (len != idx) {
				if constexpr (IsPod_v<T>) {
					::memcpy(&buf[idx], &buf[len], sizeof(T));
				} else {
					new (&buf[idx]) T((T&&)buf[len]);
				}
			}
		}

		void Clear() {
			if (!cap) return;
			if (len) {
				for (SizeType i = 0; i < len; ++i) {
					buf[i].~T();
				}
				len = 0;
			}
		}

		T const& operator[](SizeType idx) const noexcept {
			assert(idx >= 0 && idx < len);
			return buf[idx];
		}

		T& operator[](SizeType idx) noexcept {
			assert(idx >= 0 && idx < len);
			return buf[idx];
		}

		struct Iter {
			T* ptr;
			bool operator!=(Iter const& other) noexcept { return ptr != other.ptr; }
			Iter& operator++() noexcept { ++ptr; return *this; }
			T& operator*() noexcept { return *ptr; }
		};
		Iter begin() noexcept { return Iter{ buf }; }
		Iter end() noexcept { return Iter{ buf + len }; }
		Iter begin() const noexcept { return Iter{ buf }; }
		Iter end() const noexcept { return Iter{ buf + len }; }
	};


	// wrapper
	template<typename T = int32_t, typename SizeType = uint16_t, SizeType cap = 16>
	struct FixedList {
		using Pool = FixedListPool<T, SizeType, cap>;
		using Core = FixedListCore<T, SizeType, cap>;

		Core* core{};

		FixedList() = default;
		FixedList(FixedList const& o) = delete;
		FixedList& operator=(FixedList const& o) = delete;
		FixedList(FixedList&& o) : core(std::exchange(o.core, nullptr)) {};
		FixedList& operator=(FixedList&& o) {
			swap(core, o.core);
			return *this;
		}

		void Alloc(Pool& owner);

		~FixedList() noexcept {
			if (core) core->Clear();
			// todo: release core from pool
		}

		template<typename...Args>
		T& Emplace(Args&&...args) {
			assert(core);
			return core->Emplace(std::forward<Args>(args)...);
		}

		template<typename ...TS>
		void Add(TS&&...vs) {
			assert(core);
			(core->Emplace(std::forward<TS>(vs)), ...);
		}

		void RemoveAt(SizeType idx) {
			assert(core);
			core->RemoveAt(idx);
		}

		void SwapRemoveAt(SizeType idx) {
			assert(core);
			core->SwapRemoveAt(idx);
		}

		void Clear() {
			assert(core);
			core->Clear();
		}

		T const& operator[](SizeType idx) const noexcept {
			assert(core);
			return core->operator[](idx);
		}

		T& operator[](SizeType idx) noexcept {
			assert(core);
			return core->operator[](idx);
		}

		Core::Iter begin() noexcept { assert(core); return core->begin(); }
		Core::Iter end() noexcept { assert(core); return core->end(); }
		Core::Iter begin() const noexcept { assert(core); return core->begin(); }
		Core::Iter end() const noexcept { assert(core); return core->end(); }
	};

	template<typename T, typename SizeType, SizeType cap>
	struct FixedListPool {
		using Node = FixedList<T, SizeType>;
		static constexpr size_t nodeSize = sizeof(Node) + sizeof(T) * cap;
		static constexpr size_t blockSize = nodeSize * 64;

		xx::Listi32<void*> blocks;
		int32_t cap{}, len{}, freeHead{ -1 }, freeCount{};

		XX_FORCE_INLINE void Reserve() {
			cap += 64;
			blocks.Emplace(malloc(blockSize));
		}

		//XX_FORCE_INLINE void* RefBlock(int32_t index) const {
		//	assert(index >= 0 && index < this->cap);
		//	return *(Block*)this->blocks[(uint32_t&)index >> 6];
		//}

		//XX_FORCE_INLINE Node<T>& RefNode(int32_t index) const {
		//	auto& block = RefBlock(index);
		//	auto& node = block.buf[index & 0b111111];
		//	assert(node.index == index);
		//	return node;
		//}


		// todo
	};

	template<typename T, typename SizeType, SizeType cap>
	void FixedList<T, SizeType, cap>::Alloc(Pool& owner) {

	}

}


void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto basePos = rootNode->size / 2;

	auto txt = rootNode->MakeChildren<Label>();
	txt->Init(2, basePos + XY{ 0, 200 }, { 3, 3 }, { 0.5f, 0.5f }, RGBA8_White, U"astar tests");

	auto bg = rootNode->MakeChildren<Scale9Sprite>();
	bg->Init(1, basePos + XY{ 0, 200 }, { 5, 5 }, XY{ 0.5f, 0.5f }, txt->CalcBorderSize({ 50, 20 }) / 5, gLooper.s9cfg);

	rootNode->MakeChildren<Button>()->Init(3, basePos + XY{ 0, 0 }, { 0.5f, 0.5f }, gLooper.s9cfg, U"test1", [&]() {
		gLooper.DelaySwitchTo<Test1::Scene>();
	}, 3);


}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
