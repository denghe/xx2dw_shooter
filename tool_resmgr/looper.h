#pragma once
#include "pch.h"

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> DrawTask();
};

extern Looper gLooper;
