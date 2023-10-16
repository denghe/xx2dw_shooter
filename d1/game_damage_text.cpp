#include "pch.h"

void DamageText::Init(XY const& bornPos, int32_t hp) {
	Add(MainLogic());
	SetColor({255,0,0,255});
	pos = bornPos;
	txt = std::to_string(hp);
}
void DamageText::Draw() {
	gLooper.ctcDefault.Draw(pos, txt, color);
}
xx::Task<> DamageText::MainLogic() {
	XY inc{ 0, 1 };
	int32_t life{ cLife };
	do {
		co_yield 0;
		AddPosition(inc);
	} while (--life >= 0);
	constexpr int step = 255 / cLife;
	for (int i = 255; i >= 0; i -= step) {
		SetColorA((uint32_t)i);
		co_yield 0;
	}
	SetColorA(0);
}
