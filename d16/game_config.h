#pragma once
#include <game_looper.h>

namespace Config {

	struct Item {
		xx::Task<> task;

		// return true mean need dispose
		bool Update() {
			return task.Resume();
		}

		// ...
	};

	// base class
	struct Trigger : Item {
		bool GetValue() {
			return task.YieldValue().v != 0;
		}
	};

	// handle by player control
	struct Trigger_Player : Trigger {
		void Init() {
			task = MainTask();
		}
		xx::Task<> MainTask() {
			while (true) {
				co_yield gLooper.KeyDown(KeyboardKeys::Space) ? 1 : 0;
			}
		}
	};
	
	// handle by after delay, always true
	struct Trigger_Delay : Trigger {
		void Init(float delaySeconds) {
			task = MainTask(delaySeconds);
		}
		xx::Task<> MainTask(float delaySeconds) {
			for (float secs = 0; secs < delaySeconds; secs += gLooper.frameDelay) {
				co_yield 0;
			}
			while (true) {
				co_yield 1;
			}
		}
	};

	// handle by repeater
	struct Trigger_Repeater : Trigger {
		void Init(float delaySeconds, float openSeconds, float closeSeconds, int repeatTimes = std::numeric_limits<int>::max()) {
			task = MainTask(delaySeconds, openSeconds, closeSeconds, repeatTimes);
		}
		xx::Task<> MainTask(float delaySeconds, float openSeconds, float closeSeconds, int repeatTimes = std::numeric_limits<int>::max()) {
			while (delaySeconds > 0) {
				co_yield 0;
				delaySeconds -= gLooper.frameDelay;
			}
			do {
				for (float secs = 0; secs < openSeconds; secs += gLooper.frameDelay) {
					co_yield 1;
				}
				for (float secs = 0; secs < closeSeconds; secs += gLooper.frameDelay) {
					co_yield 0;
				}
			} while (--repeatTimes);
		}
	};


	// base class
	struct Emitter : Item {

		// cond: Update() == false
		xx::Shared<Emitter> GetValue() {
			xx::Shared<Emitter> r;
			r.pointer = (Emitter*)std::exchange(task.YieldValue().p, nullptr);
			return r;
		}
	};

	struct Emitter_Bullet1 : Emitter {
		static constexpr float cDamage{ 10 };
		static constexpr float cRadius{ 5 };
		static constexpr float cSpeed{ 30.f / gDesign.fps };
		static constexpr float cLifeCycle{ 2.f * gDesign.fps };

		void Init(XY const& bornPos = {}, float radians = 0.f) {
			task = MainTask(bornPos, radians);
		}

		xx::Task<> MainTask(XY bornPos, float radians) {
			// todo: assign pos, calc inc
			for (float lifeCycle = 0; lifeCycle < cLifeCycle; lifeCycle += gDesign.frameDelay) {
				// todo: pos += inc;
				co_yield 0;
			}
		}
	};

	struct Emitter_Clip : Emitter {
		static constexpr int cQuanitity{ 10 };

		void Init() {
			task = MainTask();
		}

		xx::Task<> MainTask() {
			for (int quanitity = cQuanitity; quanitity > 0; --quanitity) {
				xx::Shared<Emitter_Bullet1> b;
				b.Emplace()->Init();
				xx::CoutN("make bullet");
				co_yield { 0, b.pointer };
			}
		}
	};

	//struct Emitter_Emitter_ : Emitter {
	//	// todo
	//};

	// Emitter_Emitter_Clip ?

	// Transmitter ?

	struct Gun1 : Item {
		xx::Shared<Trigger> trigger;
		xx::Shared<Emitter> emitter;

		void Init() {
			task = MainTask();
			trigger.Emplace<Trigger_Delay>()->Init(0);
			emitter.Emplace<Emitter_Clip>()->Init();
		}

		xx::Task<> MainTask() {
			xx::Listi32<xx::Shared<Emitter>> subEmitters;	// todo: change container to DoubleLink?
			while (true) {
				if (trigger->GetValue()) {
					if (emitter->Update()) co_return;	// todo: dispose callback?
					if (auto e = emitter->GetValue()) {
						subEmitters.Emplace(std::move(e));
					}
				}
				for (int i = subEmitters.len - 1; i >= 0; --i) {
					if (subEmitters[i]->Update()) {
						subEmitters.SwapRemoveAt(i);
					}
				}
				co_yield 0;
			}
		}
	};

	inline void TestGun1() {
		auto o = xx::MakeShared<Gun1>();
		o->Init();
		for (int i = 0; i < 20; i++) {
			xx::CoutN("i = ", i);
			if (o->Update()) 
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
