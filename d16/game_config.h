#pragma once
#include <game_looper.h>

namespace Config {

	// 2^n flag
	enum class ItemTypes : uint32_t {
		Instance = 1,
		Trigger = 2,
		Emitter = 4,
		//...
	};

	inline ItemTypes operator|(ItemTypes const& a, ItemTypes const& b) {
		return (ItemTypes)((uint32_t)a | (uint32_t)b);
	}


	/*******************************************************************************************/
	/*******************************************************************************************/

	struct Item;
	struct ItemManager {
		using Items = xx::ListDoubleLink<xx::Shared<Item>, int32_t, uint32_t>;
		Items items;
		bool Update();
		void AddChild(Item* tar);
		void RemoveChild(Item* tar);
	};

	struct Item {
		virtual ~Item() {};

		// fill by init / addchild
		ItemManager* owner{};
		ItemManager::Items::IndexAndVersion ownerIV{};
		ItemTypes type{};

		// for trigger
		bool isOpen {};
		//xx::Shared<Item> emitResult;

		int lineNumber{};
		virtual int UpdateCore() {
			COR_BEGIN
				;// COR_YIELD
			COR_END
		};

		// helpers

		XX_FORCE_INLINE bool Update() {
			lineNumber = UpdateCore();
			// todo: auto remove from item manager
			return lineNumber == 0;
		}

		XX_FORCE_INLINE void RemoveFromOwner() {
			owner->RemoveChild(this);
		}
	};

	inline bool ItemManager::Update() {
		items.Foreach([&](xx::Shared<Item>& o)->bool {
			return o->Update();
		});
		return items.Empty();
	}

	inline XX_FORCE_INLINE void ItemManager::AddChild(Item* tar) {
		auto p = &items.AddCore();
		assert(tar->owner == this);
		tar->ownerIV = items.Tail();
		new (p) xx::Shared<Item>(xx::SharedFromThis(tar));
	}

	inline XX_FORCE_INLINE void ItemManager::RemoveChild(Item* tar) {
		assert(tar->owner == this);
		assert(items.At(tar->ownerIV).pointer == tar);
		auto iv = tar->ownerIV;
		tar->ownerIV = {};
		auto r = items.Remove(iv);
		assert(r);
	}

	/*******************************************************************************************/
	/*******************************************************************************************/

	struct Trigger : Item {
		float delaySeconds{}, openSeconds{}, closeSeconds{};
		int repeatTimes{};
		float secs{};

		void Init(ItemManager* owner_, float delaySeconds_ = 0, float openSeconds_ = 9999999, float closeSeconds_ = 0, int repeatTimes_ = std::numeric_limits<int>::max()) {
			assert(!lineNumber);
			owner = owner_;
			type = ItemTypes::Trigger;
			owner->AddChild(this);
			delaySeconds = delaySeconds_;
			openSeconds = openSeconds_;
			closeSeconds = closeSeconds_;
			repeatTimes = repeatTimes_;
			isOpen = false;
		}

		virtual int UpdateCore() override {
			COR_BEGIN
			for (secs = 0; secs < delaySeconds; secs += gLooper.frameDelay) {
				COR_YIELD
			}
			do {
				isOpen = true;
				for (secs = 0; secs < openSeconds; secs += gLooper.frameDelay) {
					COR_YIELD
				}
				isOpen = false;
				for (secs = 0; secs < closeSeconds; secs += gLooper.frameDelay) {
					COR_YIELD
				}
			} while (--repeatTimes);
			COR_END
		}
	};

	/*******************************************************************************************/
	/*******************************************************************************************/

	struct Bullet1 : Item {
		static constexpr float cDamage{ 10 };
		static constexpr float cRadius{ 5 };
		static constexpr float cFrameSpeed{ 30.f / gDesign.fps };
		static constexpr float cLifeCycle{ 0.5f };

		XY bornPos{}, pos{};
		float radians{};
		float lifeCycle{};

		// todo: args
		void Init(ItemManager* owner_) {
			assert(!lineNumber);
			owner = owner_;
			type = ItemTypes::Instance;
			owner->AddChild(this);
			xx::CoutN("Bullet1.Init()");
		}

		virtual int UpdateCore() override {
			COR_BEGIN
			// todo: assign pos, calc inc
			for (lifeCycle = 0; lifeCycle < cLifeCycle; lifeCycle += gDesign.frameDelay) {
				// todo: pos += inc;
				xx::CoutN("Bullet1.UpdateCore() lifeCycle = ", lifeCycle);
				COR_YIELD
			}
			xx::CoutN("Bullet1.UpdateCore() end");
			COR_END
		}
	};

	/*******************************************************************************************/
	/*******************************************************************************************/

	struct Gun1 : Item {
		xx::Weak<Item> trigger;
		static constexpr int cQuanitity{ 5 };
		static constexpr float cCastDelay{ 0.1f };
		int quanitity{};
		float secs{};

		void Init(ItemManager* owner_) {
			assert(!lineNumber);
			owner = owner_;
			type = ItemTypes::Instance | ItemTypes::Emitter;
			owner->AddChild(this);
			{
				auto t = xx::MakeShared<Trigger>();
				t->Init(owner);
				trigger = t;
			}
			quanitity = cQuanitity;
		}

		virtual int UpdateCore() override {
			COR_BEGIN
			while (true) {
				assert(trigger);
				if (trigger->isOpen) {
					if (quanitity > 0) {
						--quanitity;
						{
							auto e = xx::MakeShared<Bullet1>();
							e->Init(owner);
						}
						for (secs = 0; secs < cCastDelay; secs += gDesign.frameDelay) {
							COR_YIELD
						}
					}
				}
				COR_YIELD
			}
			COR_END
		}
	};

	/*******************************************************************************************/
	/*******************************************************************************************/

	inline void TestGun1() {
		ItemManager im;
		auto o = xx::MakeShared<Gun1>();
		o->Init(&im);

		for (int i = 0; i < gDesign.fps * 2; i++) {
			xx::CoutN("i = ", i);
			if (im.Update()) 
				break;
		}
		xx::CoutN("after for");
	}
}

//enum class WeaponTypes {
//	Unknown,
//	Particle,		// fly instance
//	Wave,			// close combat shape? explosion? sound? ( fan diffusion attack )
//	Lightning,		// teleport electronic explosion ( teleport range attack )
//	Laser,			// energy / high brightness light ( line attack )
//	Burn,			// fire dot attack
//	Curse,			// debuff attack
//	Summon			// summon friendly forces or resurrect corpses?
//};

//// 3 types: emitter, transfer, effector

////struct WeaponConfig {
////	WeaponTypes cType{};
////	float cDamage{};
////	float cCastDelaySeconds{};
////};



//namespace Config {
//
//	struct Item {
//		xx::Task<> task;
//
//		// return true mean need dispose
//		bool Update() {
//			return task.Resume();
//		}
//
//		// ...
//	};
//
//	// base class
//	struct Trigger : Item {
//		bool IsOpen() {
//			return task.YieldValue().v != 0;
//		}
//	};
//
//	// handle by player control
//	struct Trigger_Player : Trigger {
//		void Init() {
//			task = MainTask();
//		}
//		xx::Task<> MainTask() {
//			while (true) {
//				co_yield gLooper.KeyDown(KeyboardKeys::Space) ? 1 : 0;
//			}
//		}
//	};
//	
//	// handle by after delay, always true
//	struct Trigger_Delay : Trigger {
//		void Init(float delaySeconds) {
//			task = MainTask(delaySeconds);
//		}
//		xx::Task<> MainTask(float delaySeconds) {
//			for (float secs = 0; secs < delaySeconds; secs += gLooper.frameDelay) {
//				co_yield 0;
//			}
//			while (true) {
//				co_yield 1;
//			}
//		}
//	};
//
//	// handle by repeater
//	struct Trigger_Repeater : Trigger {
//		void Init(float delaySeconds, float openSeconds, float closeSeconds, int repeatTimes = std::numeric_limits<int>::max()) {
//			task = MainTask(delaySeconds, openSeconds, closeSeconds, repeatTimes);
//		}
//		xx::Task<> MainTask(float delaySeconds, float openSeconds, float closeSeconds, int repeatTimes = std::numeric_limits<int>::max()) {
//			while (delaySeconds > 0) {
//				co_yield 0;
//				delaySeconds -= gLooper.frameDelay;
//			}
//			do {
//				for (float secs = 0; secs < openSeconds; secs += gLooper.frameDelay) {
//					co_yield 1;
//				}
//				for (float secs = 0; secs < closeSeconds; secs += gLooper.frameDelay) {
//					co_yield 0;
//				}
//			} while (--repeatTimes);
//		}
//	};
//
//
//	// base class
//	struct Emitter : Item {
//
//		// cond: Update() == false
//		xx::Shared<Emitter> GetValue() {
//			xx::Shared<Emitter> r;
//			r.pointer = (Emitter*)std::exchange(task.YieldValue().p, nullptr);
//			return r;
//		}
//	};
//
//	struct Emitter_Bullet1 : Emitter {
//		static constexpr float cDamage{ 10 };
//		static constexpr float cRadius{ 5 };
//		static constexpr float cSpeed{ 30.f / gDesign.fps };
//		static constexpr float cLifeCycle{ 2.f * gDesign.fps };
//
//		void Init(XY const& bornPos = {}, float radians = 0.f) {
//			task = MainTask(bornPos, radians);
//		}
//
//		xx::Task<> MainTask(XY bornPos, float radians) {
//			// todo: assign pos, calc inc
//			for (float lifeCycle = 0; lifeCycle < cLifeCycle; lifeCycle += gDesign.frameDelay) {
//				// todo: pos += inc;
//				co_yield 0;
//			}
//		}
//	};
//
//	struct Emitter_Clip : Emitter {
//		static constexpr int cQuanitity{ 10 };
//
//		void Init() {
//			task = MainTask();
//		}
//
//		xx::Task<> MainTask() {
//			for (int quanitity = cQuanitity; quanitity > 0; --quanitity) {
//				xx::Shared<Emitter_Bullet1> b;
//				b.Emplace()->Init();
//				xx::CoutN("make bullet");
//				co_yield { 0, b.pointer };
//			}
//		}
//	};
//
//	//struct Emitter_Emitter_ : Emitter {
//	//	// todo
//	//};
//
//	// Emitter_Emitter_Clip ?
//
//	// Transmitter ?
//
//	struct Gun1 : Item {
//		xx::Shared<Trigger> trigger;
//		xx::Shared<Emitter> emitter;
//
//		void Init() {
//			task = MainTask();
//			trigger.Emplace<Trigger_Delay>()->Init(0);
//			emitter.Emplace<Emitter_Clip>()->Init();
//		}
//
//		xx::Task<> MainTask() {
//			xx::Listi32<xx::Shared<Emitter>> subEmitters;	// todo: change container to DoubleLink?
//			while (true) {
//				trigger->Update();
//				if (trigger->IsOpen()) {
//					if (emitter->Update()) co_return;	// todo: dispose callback?
//					if (auto e = emitter->GetValue()) {
//						subEmitters.Emplace(std::move(e));
//					}
//				}
//				for (int i = subEmitters.len - 1; i >= 0; --i) {
//					if (subEmitters[i]->Update()) {
//						subEmitters.SwapRemoveAt(i);
//					}
//				}
//				co_yield 0;
//			}
//		}
//	};
//
//	inline void TestGun1() {
//		auto o = xx::MakeShared<Gun1>();
//		o->Init();
//		for (int i = 0; i < 20; i++) {
//			xx::CoutN("i = ", i);
//			if (o->Update()) 
//				break;
//		}
//		xx::CoutN("after for");
//	}
//
//}
