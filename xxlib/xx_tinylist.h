#pragma once
#include "xx_typetraits.h"

namespace xx {

	template<typename T, typename SizeType = int32_t>
	struct TinyList {
		struct Core {
			SizeType len, cap;
			T buf[0];
		} *core{};

		TinyList() = default;
		TinyList(TinyList const& o) = delete;
		TinyList& operator=(TinyList const& o) = delete;
		TinyList(TinyList&& o) noexcept : core(std::exchange(o.core, nullptr)) {}
		TinyList& operator=(TinyList&& o) noexcept {
			std::swap(core, o.core);
			return *this;
		}
		~TinyList() noexcept {
			Clear(true);
		}

		bool Empty() const {
			return !core || !core->len;
		}

		SizeType Count() const {
			return !core ? 0 : core->len;
		}

		T* Buf() const {
			return core ? (T*)core->buf : nullptr;
		}

		SizeType Len() const {
			return core ? core->len : 0;
		}

		TinyList Clone() const {
			TinyList rtv;
			rtv.Reserve(core->len);
			rtv.AddRange(*this);
			return rtv;
		}

		// func == [](auto& a, auto& b) { return a->xxx < b->xxx; }
		template<typename F>
		XX_FORCE_INLINE void StdSort(F&& func) {
			if (!core) return;
			std::sort(core->buf, core->buf + core->len, std::forward<F>(func));
		}

		void Reserve(SizeType cap_) noexcept {
			if (auto newBuf = ReserveBegin(cap_)) {
				ReserveEnd(newBuf);
			}
		}

		Core* ReserveBegin(SizeType cap_) noexcept {
			assert(cap_ > 0);
			if (core && cap_ <= core->cap) return {};
			if (!core) {
				core = (Core*)malloc(sizeof(Core) + sizeof(T) * cap_);
				core->len = 0;
				core->cap = cap_;
				return {};
			}
			auto newCap = core->cap;
			do {
				newCap *= 2;
			} while (newCap < cap_);

			auto newCore = (Core*)malloc(sizeof(Core) + sizeof(T) * newCap);
			newCore->len = core->len;
			newCore->cap = core->cap;
			return newCore;
		}

		void ReserveEnd(Core* newCore) noexcept {
			auto& buf = core->buf;
			auto& len = core->len;
			auto& newBuf = newCore->buf;
			if constexpr (IsPod_v<T>) {
				::memcpy((void*)newBuf, (void*)buf, len * sizeof(T));
			} else {
				for (SizeType i = 0; i < len; ++i) {
					std::construct_at(&newBuf[i], std::move(buf[i]));
					std::destroy_at(&buf[i]);
				}
			}
			::free(core);
			core = newCore;
		}

		template<bool fillVal = false, int val = 0>
		void Resize(SizeType len_) noexcept {
			if (len_ == core->len) return;
			else if (len_ < core->len) {
				for (SizeType i = len_; i < core->len; ++i) {
					std::destroy_at(&core->buf[i]);
				}
			} else {	// len_ > len
				Reserve(len_);
				if constexpr (!(std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
					for (SizeType i = core->len; i < len_; ++i) {
						std::construct_at(&core->buf[i]);
					}
				} else if constexpr (fillVal) {
					memset(core->buf + core->len, val, (len_ - core->len) * sizeof(T));
				}
			}
			core->len = len_;
		}

		T const& operator[](SizeType idx) const noexcept {
			assert(core && idx >= 0 && idx < core->len);
			return core->buf[idx];
		}
		T& operator[](SizeType idx) noexcept {
			assert(core && idx >= 0 && idx < core->len);
			return core->buf[idx];
		}
		T const& At(SizeType idx) const noexcept {
			xx_assert(core && idx >= 0 && idx < core->len);
			return core->buf[idx];
		}
		T& At(SizeType idx) noexcept {
			xx_assert(core && idx >= 0 && idx < core->len);
			return core->buf[idx];
		}

		T& Top() noexcept {
			assert(core && core->len > 0);
			return core->buf[core->len - 1];
		}
		void Pop() noexcept {
			assert(core && core->len > 0);
			--core->len;
			std::destroy_at(&core->buf[core->len]);
		}
		T const& Top() const noexcept {
			assert(core && core->len > 0);
			return core->buf[core->len - 1];
		}
		bool TryPop(T& output) noexcept {
			if (!core || !core->len) return false;
			output = std::move(core->buf[--core->len]);
			std::destroy_at(&core->buf[core->len]);
			return true;
		}

		void Clear(bool freeBuf = false) noexcept {
			if (!core) return;
			if (core->len) {
				for (SizeType i = 0, len = core->len; i < len; ++i) {
					std::destroy_at(&core->buf[i]);
				}
				core->len = 0;
			}
			if (freeBuf) {
				::free(core);
				core = {};
			}
		}

		void Remove(T const& v) noexcept {
			assert(core);
			auto& len = core->len;
			auto& buf = core->buf;
			for (SizeType i = 0; i < len; ++i) {
				if (v == buf[i]) {
					RemoveAt(i);
					return;
				}
			}
		}

		void RemoveAt(SizeType idx) {
			assert(core);
			auto& len = core->len;
			auto& buf = core->buf;
			assert(idx >= 0 && idx < len);
			--len;
			std::destroy_at(&buf[idx]);
			if constexpr (IsPod_v<T>) {
				::memmove(buf + idx, buf + idx + 1, (len - idx) * sizeof(T));
			} else {
				for (SizeType i = idx; i < len; ++i) {
					std::construct_at(&buf[i], std::move(buf[i + 1]));
				}
				std::destroy_at(&buf[len]);
			}
		}

		void SwapRemoveAt(SizeType idx) {
			assert(core);
			auto& len = core->len;
			assert(idx >= 0 && idx < len);
			auto& buf = core->buf;
			std::destroy_at(&buf[idx]);
			--len;
			if (len != idx) {
				if constexpr (IsPod_v<T>) {
					::memcpy(&buf[idx], &buf[len], sizeof(T));
				} else {
					std::construct_at(&buf[idx], std::move(buf[len]));
				}
			}
		}

		XX_FORCE_INLINE void PopBack() {
			assert(core && core->len);
			--core->len;
			if constexpr (!(std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
				std::destroy_at(&core->buf[core->len]);
			}
		}

		XX_FORCE_INLINE T& Back() {
			assert(core && core->len);
			return core->buf[core->len - 1];
		}
		XX_FORCE_INLINE T const& Back() const {
			assert(core && core->len);
			return core->buf[core->len - 1];
		}

		template<typename...Args>
		T& Emplace(Args&&...args) noexcept {
			if (auto newCore = ReserveBegin(core ? core->len + 1 : 4)) {
				auto& newBuf = newCore->buf;
				auto& len = newCore->len;
				std::construct_at(&newBuf[len], std::forward<Args>(args)...);
				ReserveEnd(newCore);
				return newBuf[len++];
			} else {
				return *std::construct_at(&core->buf[core->len++], std::forward<Args>(args)...);
			}
		}

		template<typename ...TS>
		void Add(TS&&...vs) noexcept {
			(Emplace(std::forward<TS>(vs)), ...);
		}

		void AddRange(T const* items, SizeType count) noexcept {
			assert(items && count > 0);
			if (auto newCore = ReserveBegin(core ? core->len + count : count)) {
				auto& newBuf = newCore->buf;
				auto& len = newCore->len;
				if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) {
					::memcpy(newBuf + len, items, count * sizeof(T));
				} else {
					for (SizeType i = 0; i < count; ++i) {
						std::construct_at(&newBuf[len + i], items[i]);
					}
				}
				ReserveEnd(newBuf);
				len += count;
			} else {
				auto& buf = core->buf;
				auto& len = core->len;
				if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) {
					::memcpy(buf + len, items, count * sizeof(T));
				} else {
					for (SizeType i = 0; i < count; ++i) {
						std::construct_at(&buf[len + i], items[i]);
					}
				}
				len += count;
			}
		}

		template<typename L>
		void AddRange(L const& list) noexcept {
			return AddRange(list.Buf(), list.Len());
		}

		SizeType Find(T const& v) const noexcept {
			for (SizeType i = 0; i < core->len; ++i) {
				if (v == core->buf[i]) return i;
			}
			return SizeType(-1);
		}

		template<typename Func>
		bool Exists(Func&& cond) const noexcept {
			for (SizeType i = 0; i < core->len; ++i) {
				if (cond(core->buf[i])) return true;
			}
			return false;
		}

		struct Iter {
			T* ptr;
			bool operator!=(Iter const& other) noexcept { return ptr != other.ptr; }
			Iter& operator++() noexcept { ++ptr; return *this; }
			T& operator*() noexcept { return *ptr; }
		};
		Iter begin() noexcept { assert(core); return Iter{ core->buf }; }
		Iter end() noexcept { assert(core); return Iter{ core->buf + core->len }; }
		Iter begin() const noexcept { assert(core); return Iter{ core->buf }; }
		Iter end() const noexcept { assert(core); return Iter{ core->buf + core->len }; }

	};

}
