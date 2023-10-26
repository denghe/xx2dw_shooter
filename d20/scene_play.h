#pragma once
#include <game_looper.h>
#include <hero.h>
#include <grass.h>

struct Light : Quad {
	FrameBuffer fb;

	void Init() {
		fb.Init();
		auto tex = FrameBuffer::MakeTexture(EngineBase1::Instance().windowSize.As<uint32_t>());
		SetFrame(Frame::Create(tex));
	}

	template<typename F>
	void Draw(F&& func) {
		auto& e = EngineBase1::Instance();
		fb.DrawTo(frame->tex, RGBA8{ 50,50,50,255 }, [&] {
			e.GLBlendFunc({ GL_SRC_ALPHA, GL_ONE, GL_FUNC_ADD });
			func();
		});
		auto bak = e.blend;
		e.GLBlendFunc({ GL_DST_COLOR, GL_ZERO, GL_FUNC_ADD });
		Quad::Draw();
		e.GLBlendFunc(bak);
	}
};

struct ScenePlay : Scene {
	xx::Shared<Node> rootNode;
	Quad symbolQuad;

	Light light;
	Grass grass;
	xx::List<xx::Ref<Hero>, int32_t> heroes;

	virtual void Init() override;
	virtual void Draw() override;
};
