#pragma once
#include "xx_space.h"

namespace xx {

	struct SpacesNodeBase : BlockLinkVI {
		int32_t nex, pre, cidx, typeId;
	};

	template<typename T>
	struct SpacesNode : SpacesNodeBase {
		T value;
	};

	template<typename BT>
	struct SpacesWeak {
		BT* pointer{};
		int32_t version{ -2 }, offset{ -1 };

		SpacesWeak() = default;
		SpacesWeak(SpacesWeak const&) = default;
		SpacesWeak& operator=(SpacesWeak const&) = default;
		SpacesWeak(SpacesWeak&&) = default;
		SpacesWeak& operator=(SpacesWeak&&) = default;

		template<std::derived_from<BT> T>
		SpacesWeak(T const& v) {
			operator=(v);
		}

		template<std::derived_from<BT> T>
		SpacesWeak& operator=(T const& v) {
			pointer = (BT*)static_cast<BT const*>(&v);
			auto o = container_of(&v, SpacesNode<T>, value);
			version = o->version;
			offset = (int32_t)((char*)&v - (char*)o);
			return *this;
		}

		XX_FORCE_INLINE SpacesNodeBase& RefNodeBase() const {
			return *(SpacesNodeBase*)((char*)pointer - offset);
		}

		XX_FORCE_INLINE bool Exists() const noexcept {
			return pointer && version < -2 && version == RefNodeBase().version;
		}
		XX_FORCE_INLINE operator bool() const noexcept {
			return Exists();
		}

		XX_FORCE_INLINE BT& operator()() const {
			assert(Exists());
			return (BT&)*pointer;
		}

		XX_FORCE_INLINE void Reset() {
			pointer = {};
			version = -2;
			offset = -1;
		}

		XX_FORCE_INLINE int32_t TypeId() const {
			assert(Exists());
			return RefNodeBase().typeId;
		}

		template<std::derived_from<BT> T>
		XX_FORCE_INLINE T& As() {
			assert(Exists());
			assert(dynamic_cast<T*>(pointer));
			return (T&)pointer;
		}
	};


	template<typename BT, std::derived_from<BT>...TS>
	struct SpaceGrids {
		using Tup = std::tuple<TS...>;
		static constexpr std::array<size_t, sizeof...(TS)> ts{ xx::TupleTypeIndex_v<TS, Tup>... };
		static constexpr std::array<size_t, sizeof...(TS)> is{ TS::cTypeId... };
		static_assert(ts == is);	// ensure cTypeId == index

		template<std::derived_from<BT> T>
		using SG = SpaceGrid<T, BlockLink<T, SpacesNode>>;
		using SGS = xx::SimpleTuple<SG<TS>...>;
		using WeakType = SpacesWeak<BT>;

		SGS sgs;
		static_assert(sizeof(sgs.value) * sizeof...(TS) == sizeof(sgs));	// ensure array mode visit

		//// sizes for calculate offsets
		//static constexpr std::array<size_t, sizeof...(TS)> sizes{ sizeof(TS)... };

		// for Remove
		typedef void(*Remover)(void* sg, void* o);
		std::array<Remover, sizeof...(TS)> removers;

		SpaceGrids() {
			xx::ForEachType<Tup>([&]<typename T>() {
				removers[xx::TupleTypeIndex_v<T, Tup>] = [](void* sg, void* o) {
					((SG<T>*)sg)->Remove(*(BlockLinkVI*)o);
				};
			});
		}
		SpaceGrids(SpaceGrids const&) = delete;
		SpaceGrids& operator=(SpaceGrids const&) = delete;

		template<typename T>
		SG<T>& Get() const {
			return ::xx::Get<SG<T>>((SGS&)sgs);
		}

		// grids.Foreach([&]<typename T>( decltype(grids)::SG<T>& grid) {   });
		template<typename F>
		void Foreach(F&& func) {
			(func(Get<TS>()), ...);
		}

		template<typename ...US>
		void Init(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
			xx::ForEachType<Tup>([&]<typename T>() {
				Get<T>().Init(numRows_, numCols_, cellSize_);
			});
		}

		void InitAll(int32_t numRows_, int32_t numCols_, int32_t cellSize_) {
			Init<TS...>(numRows_, numCols_, cellSize_);
		}

		template<typename T, typename...Args>
		T& EmplaceInit(Args&&...args) {
			return Get<T>().EmplaceInit(std::forward<Args>(args)...);
		}
		
		template<typename T, typename...Args>
		typename SG<T>::NodeType& EmplaceNodeInit(Args&&...args) {
			return Get<T>().EmplaceNodeInit(std::forward<Args>(args)...);
		}

		template<typename T>
		void Update(T& o) {
			Get<T>().Update(o);
		}

		template<typename T>
		void Remove(T& o) {
			Get<T>().Remove(o);
		}

		// todo: test
		void Remove(SpacesWeak<BT> const& w) {
			if (!w.pointer || w.version >= -2) return;
			auto& o = w.RefNodeBase();
			if (w.version != o.version) return;
			auto& sg = ((SG<BT>*)&sgs)[o.typeId];
			removers[o.typeId](&sg, &o);
		}

		int32_t Count() {
			return (Get<TS>().Count() + ...);
		}
	};
}
