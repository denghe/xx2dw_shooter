#include "pch.h"

void DamageText::Init(XY const& bornPos, int32_t hp) {
	mainLogic = MainLogic();
	color = {255,0,0,255};
	pos = bornPos;
	txt = std::to_string(hp);
}

void DamageText::Draw() {
	gLooper.ctc24.Draw(gLooper.camera.ToGLPos(pos), txt, color);		// todo: align center?
}

xx::Task<> DamageText::MainLogic() {
	// move up
	int32_t life{ cLife };
	do {
		co_yield 0;
		pos.y -= moveSpeed;
	} while (--life >= 0);

	// fade out
	constexpr int step = 255 / cLife;
	for (int i = 255; i >= 0; i -= step) {
		color.a = (uint32_t)i;
		co_yield 0;
	}
	color.a = 0;
}
