#pragma once
#include <engine_base1.h>

struct Scissor {
	XY bakWndSiz{};
	std::array<uint32_t, 3> bakBlend;

	template<typename Func>
	void DrawTo(XY const& pos, XY const& wh, Func&& func) {
		Begin(pos.x, pos.y, wh.x, wh.y);
		func();
		End();
	}

protected:
	XX_FORCE_INLINE void Begin(float x, float y, float w, float h) {
		auto& eb = EngineBase1::Instance();
		eb.ShaderEnd();
		bakWndSiz = eb.windowSize;
		bakBlend = eb.blend;
		Vec2<GLsizei> wp{ GLsizei(eb.windowSize_2.x + x), GLsizei(eb.windowSize_2.y + y) };
		eb.SetWindowSize(w, h);
		eb.GLBlendFunc(eb.blendDefault);
		glViewport(wp.x, wp.y, (GLsizei)w, (GLsizei)h);
		glScissor(wp.x, wp.y, (GLsizei)w, (GLsizei)h);
		glEnable(GL_SCISSOR_TEST);
	}

	XX_FORCE_INLINE void End() {
		auto& eb = EngineBase1::Instance();
		eb.ShaderEnd();
		glDisable(GL_SCISSOR_TEST);
		eb.SetWindowSize(bakWndSiz.x, bakWndSiz.y);
		eb.GLViewport();
		eb.GLBlendFunc(bakBlend);
	}
};
