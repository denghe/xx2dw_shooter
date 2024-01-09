#pragma once
#include <game_looper.h>

namespace Config {

	struct Item;
	struct ItemManager {
		xx::ListLink<xx::Shared<Item>, int32_t> items;
		bool Update();
	};

	struct Item {
		virtual ~Item() {};

		ItemManager* im{};
		XX_FORCE_INLINE void ItemInit(ItemManager* im_) {
			assert(!im);
			assert(im_);
			assert(!lineNumber);
			im = im_;
			im->items.Emplace(xx::SharedFromThis(this));
		}

		int lineNumber{};
		virtual int UpdateCore() {
			return 0;	// COR_BEGIN COR_YIELD COR_EXIT COR_END 
		};

		XX_FORCE_INLINE bool Update() {
			lineNumber = UpdateCore();
			return lineNumber == 0;		// will remove by caller
		}
	};

	inline bool ItemManager::Update() {
		items.Foreach([&](xx::Shared<Item>& o)->bool {
			return o->Update();
		});
		return items.Empty();
	}

	/*******************************************************************************************/
	/*******************************************************************************************/

	struct Trigger : Item {
		xx::Weak<Item> owner;	// Conditions for existence

		float delaySeconds{}, openSeconds{}, closeSeconds{};
		int repeatTimes{};
		float secs{};
		bool isOpen{};

		xx::Weak<Trigger> Init(ItemManager* im_, xx::Weak<Item> owner_, float delaySeconds_ = 0, float openSeconds_ = 9999999, float closeSeconds_ = 0, int repeatTimes_ = std::numeric_limits<int>::max()) {
			ItemInit(im_);

			owner = std::move(owner_);
			delaySeconds = delaySeconds_;
			openSeconds = openSeconds_;
			closeSeconds = closeSeconds_;
			repeatTimes = repeatTimes_;
			xx::CoutN("Trigger.Init()");

			return xx::WeakFromThis(this);
		}

		virtual int UpdateCore() override {
			if (!owner) {
				xx::CoutN("Trigger.UpdateCore() end");
				return 0;
			} else {
				xx::CoutN("Trigger.UpdateCore()");
			}
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

		XY pos{};
		float radius{};
		float radians{};
		float damage{};

		void Init(ItemManager* im_) {
			// todo: args
			ItemInit(im_);
			xx::CoutN("Bullet1.Init()");
		}

		virtual int UpdateCore() override {
			COR_BEGIN
				//// todo: assign pos, calc inc
				//for (lifeCycle = 0; lifeCycle < cLifeCycle; lifeCycle += gDesign.frameDelay) {
				//	// todo: pos += inc;
				//	xx::CoutN("Bullet1.UpdateCore() lifeCycle = ", lifeCycle);
				//	COR_YIELD
				//}
				//xx::CoutN("Bullet1.UpdateCore() end");
					// todo: hit check ?
				;
			COR_END
		}
	};

	template<typename BT = Bullet1>
	struct Propeller : Item {
		// pathway ?
		float lifeCycle{};
		xx::Weak<BT> target;

		void Init(ItemManager* im_) {
			// todo: args
			ItemInit(im_);
			xx::CoutN("Propeller.Init()");
		}

		virtual int UpdateCore() override {
			COR_BEGIN
				//// todo: assign pos, calc inc
				//for (lifeCycle = 0; lifeCycle < cLifeCycle; lifeCycle += gDesign.frameDelay) {
				//	// todo: pos += inc;
				//	xx::CoutN("Bullet1.UpdateCore() lifeCycle = ", lifeCycle);
				//	COR_YIELD
				//}
				//xx::CoutN("Bullet1.UpdateCore() end");
					// todo: hit check ?
				;
			COR_END
		}
	};

	/*******************************************************************************************/
	/*******************************************************************************************/

	template<typename BT = Bullet1>
	struct Gun1 : Item {
		static constexpr int cQuanitity{ 5 };
		static constexpr float cMinCastDelay{ 0.1f };

		xx::Weak<Trigger> trigger;	// emit condition
		int quanitity{};
		float secs{};

		void Init(ItemManager* im_) {
			ItemInit(im_);

			trigger = xx::MakeShared<Trigger>()->Init(im, xx::WeakFromThis(this), 0.2f, 0.01f, 0.15f);
			quanitity = cQuanitity;
			xx::CoutN("Gun1.Init()");
		}

		virtual int UpdateCore() override {
			COR_BEGIN
			while (true) {
				assert(trigger);
				if (trigger->isOpen) {
					if (quanitity > 0) {
						--quanitity;
						{
							auto e = xx::MakeShared<BT>();
							e->Init(im);
						}
						for (secs = 0; secs < cMinCastDelay; secs += gDesign.frameDelay) {
							COR_YIELD
						}
					} else {
						xx::CoutN("Gun1.UpdateCore() end");
						COR_EXIT;
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
		xx::MakeShared<Gun1<Bullet1>>()->Init(&im);

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

	//inline XX_FORCE_INLINE void ItemManager::AddChild(Item* tar) {
	//	auto p = &items.AddCore();
	//	assert(tar->im == this);
	//	//tar->ownerIV = items.Tail();
	//	new (p) xx::Shared<Item>(xx::SharedFromThis(tar));
	//}

	//inline XX_FORCE_INLINE void ItemManager::RemoveChild(Item* tar) {
	//	assert(tar->im == this);
	//	assert(items.At(tar->ownerIV).pointer == tar);
	//	auto iv = tar->ownerIV;
	//	tar->ownerIV = {};
	//	auto r = items.Remove(iv);
	//	assert(r);
	//}
		//XX_FORCE_INLINE void RemoveFromOwner() {
		//	im->RemoveChild(this);
		//}


	//// 2^n flag
	//enum class ItemTypes : uint32_t {
	//	Instance = 1,
	//	Trigger = 2,
	//	Emitter = 4,
	//	//...
	//};

	//inline ItemTypes operator|(ItemTypes const& a, ItemTypes const& b) {
	//	return (ItemTypes)((uint32_t)a | (uint32_t)b);
	//}
// ItemTypes type{};
// , ItemTypes type_
// type = type_;
// , ItemTypes::Trigger
// , ItemTypes::Instance
// , ItemTypes::Instance | ItemTypes::Emitter
