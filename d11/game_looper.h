#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct KeyComboTest;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();
	bool ready{};

	xx::Shared<Frame> frame_button;
	xx::Shared<Node> root;

	xx::Shared<KeyComboTest> kct;
};
extern GameLooper gLooper;

// priority by value
enum class Combos {
	asd, dsd, sd,
};

constexpr std::array<char const*, 3> Combos_txt = {
	"ASD J", "DSD J", "SD J"
};

struct KeyComboTest {
	constexpr static float cTimeoutSecs = 0.2f;
	constexpr static float cCastSecs = 1;
	bool casting{};
	xx::List<Combos, int32_t> matchResults;
	std::string msg;

	bool DetectKeys(std::initializer_list<KeyboardKeys> allow, std::initializer_list<KeyboardKeys> deny = {}) {
		for (auto& k : allow) {
			if (!gLooper.KeyDown(k)) return false;
		}
		for (auto& k : deny) {
			if (gLooper.KeyDown(k)) return false;
		}
		return true;
	}

	xx::Task<bool> DetectKeys(float timeout, std::initializer_list<KeyboardKeys> allow, std::initializer_list<KeyboardKeys> deny = {}) {
		for (auto endTime = gLooper.nowSecs + timeout; endTime > gLooper.nowSecs;) {
			for (auto& k : allow) {
				if (!gLooper.KeyDown(k)) goto LabContinue;
			}
			for (auto& k : deny) {
				if (gLooper.KeyDown(k)) goto LabContinue;
			}
			co_return true;
		LabContinue:
			co_yield 0;
		}
		co_return false;
	}

	xx::Task<> Cast(Combos c) {
		if (casting) co_return;
		matchResults.Add(c);
		co_yield 0;													// wait sort & select by priority
		if (matchResults.Find(c) == -1) co_return;
		matchResults.Clear();

		casting = true;
		auto sgCasting = xx::MakeSimpleScopeGuard([this] { casting = false; });

		for (auto endTime = gLooper.nowSecs + cCastSecs; endTime > gLooper.nowSecs;) {
			msg = std::string(Combos_txt[(int)c]) + " casted...";
			co_yield 0;
		}
		msg.clear();
	}

	xx::TaskGuard matchASD;
	xx::Task<> MatchASD() {
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::S }, { KeyboardKeys::D })) co_return;
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::D })) co_return;
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::J })) co_return;
		co_await Cast(Combos::asd);
	}

	xx::TaskGuard matchSD;
	xx::Task<> MatchSD() {
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::D })) co_return;
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::J })) co_return;
		co_await Cast(Combos::sd);
	}

	xx::TaskGuard matchDSD;
	xx::Task<> MatchDSD() {
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::S }, { KeyboardKeys::D })) co_return;
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::D })) co_return;
		if (!co_await DetectKeys(cTimeoutSecs, { KeyboardKeys::J })) co_return;
		co_await Cast(Combos::dsd);
	}

	void Update() {
		if (!matchASD && DetectKeys({ KeyboardKeys::A }, { KeyboardKeys::S, KeyboardKeys::D })) {
			matchASD(gLooper.tasks, MatchASD());
		}
		if (!matchSD && DetectKeys({ KeyboardKeys::S }, { KeyboardKeys::D })) {
			matchSD(gLooper.tasks, MatchSD());
		}
		if (!matchDSD && DetectKeys({ KeyboardKeys::D }, { KeyboardKeys::S })) {
			matchDSD(gLooper.tasks, MatchDSD());
		}

		if (!matchResults.Empty()) {
			std::sort(matchResults.buf, matchResults.buf + matchResults.len);
			matchResults.Resize(1);
		}
	}
};
