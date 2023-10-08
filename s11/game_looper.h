#pragma once
#include "engine_engine.h"

int32_t main();

constexpr static GDesign<1280, 720, 60> gDesign;

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

struct KeyComboTest {
	constexpr static float cTimeoutSecs = 0.2;
	constexpr static float cCastSecs = 1;

	xx::TaskGuard matchASD;
	xx::Task<> MatchASD() {
		for (auto endTime = gLooper.nowSecs + cTimeoutSecs; endTime > gLooper.nowSecs;) {
			if (gLooper.KeyDown(KeyboardKeys::S) &&
				!gLooper.KeyDown(KeyboardKeys::D)) goto Lab1;
			co_yield 0;
		}
		co_return;
	Lab1:
		for (auto endTime = gLooper.nowSecs + cTimeoutSecs; endTime > gLooper.nowSecs;) {
			if (gLooper.KeyDown(KeyboardKeys::D)) goto Lab2;
			co_yield 0;
		}
		co_return;
	Lab2:
		matchResults.Add(Combos::asd);
		co_yield 0;
		if (matchResults.Find(Combos::asd) == -1) co_return;
		matchResults.Clear();
		for (auto endTime = gLooper.nowSecs + cCastSecs; endTime > gLooper.nowSecs;) {
			gLooper.ctcDefault.Draw({ 0, -100 }, "ASD fired...");
			co_yield 0;
		}
	}

	xx::TaskGuard matchSD;
	xx::Task<> MatchSD() {
		for (auto endTime = gLooper.nowSecs + cTimeoutSecs; endTime > gLooper.nowSecs;) {
			if (gLooper.KeyDown(KeyboardKeys::D)) goto Lab1;
			co_yield 0;
		}
		co_return;
	Lab1:
		matchResults.Add(Combos::sd);
		co_yield 0;
		if (matchResults.Find(Combos::sd) == -1) co_return;
		matchResults.Clear();
		for (auto endTime = gLooper.nowSecs + cCastSecs; endTime > gLooper.nowSecs;) {
			gLooper.ctcDefault.Draw({ 0, -100 }, "SD fired...");
			co_yield 0;
		}
	}

	xx::TaskGuard matchDSD;
	xx::Task<> MatchDSD() {
		for (auto endTime = gLooper.nowSecs + cTimeoutSecs; endTime > gLooper.nowSecs;) {
			if (gLooper.KeyDown(KeyboardKeys::S) &&
				!gLooper.KeyDown(KeyboardKeys::D)) goto Lab1;
			co_yield 0;
		}
		co_return;
	Lab1:
		for (auto endTime = gLooper.nowSecs + cTimeoutSecs; endTime > gLooper.nowSecs;) {
			if (gLooper.KeyDown(KeyboardKeys::D)) goto Lab2;
			co_yield 0;
		}
		co_return;
	Lab2:
		matchResults.Add(Combos::dsd);
		co_yield 0;
		if (matchResults.Find(Combos::dsd) == -1) co_return;
		matchResults.Clear();
		for (auto endTime = gLooper.nowSecs + cCastSecs; endTime > gLooper.nowSecs;) {
			gLooper.ctcDefault.Draw({ 0, -100 }, "DSD fired...");
			co_yield 0;
		}
	}

	bool HasAnyMatch() const {
		return matchASD || matchSD || matchDSD;
	}

	xx::List<Combos, int32_t> matchResults;

	void Update() {

		if (!HasAnyMatch() &&
			gLooper.KeyDown(KeyboardKeys::A) &&
			!gLooper.KeyDown(KeyboardKeys::S) &&
			!gLooper.KeyDown(KeyboardKeys::D)
			) {
			matchASD(gLooper.tasks, MatchASD());
		}

		if (!HasAnyMatch() &&
			gLooper.KeyDown(KeyboardKeys::S) &&
			!gLooper.KeyDown(KeyboardKeys::D)
			) {
			matchSD(gLooper.tasks, MatchSD());
		}

		if (!HasAnyMatch() &&
			gLooper.KeyDown(KeyboardKeys::D) &&
			!gLooper.KeyDown(KeyboardKeys::S)
			) {
			matchDSD(gLooper.tasks, MatchDSD());
		}

		if (!matchResults.Empty()) {
			std::sort(matchResults.buf, matchResults.buf + matchResults.len);
			matchResults.Resize(1);
		}
	}
};
