#pragma once
#include <game_looper.h>

struct Grass : Quad {
	static constexpr float cScale{ 4 };
	FrameBuffer fb;
	void Init() {
		auto tex = FrameBuffer::MakeTexture(gLooper.windowSize.As<uint32_t>());
		fb.Init().DrawTo(tex, RGBA8{ 0x6a, 0xbe, 0x30, 0xff }, [&] {
			xx::List<Quad, int32_t> qs;
			auto len = gLooper.frames_grass.len;
			qs.Resize(len);
			for (int32_t i = 0; i < len; i++) {
				qs[i].SetFrame(gLooper.frames_grass[i]).SetScale(cScale);
			}
			auto& r = gLooper.rnd;
			XY wh_2 = gLooper.windowSize_2;
			auto to = len - 2;
			for (size_t i = 0; i < 10000; i++) {
				auto& q = qs[r.Next<int32_t>(to)];
				XY pos{ r.Next<float>(-wh_2.x, wh_2.x), r.Next<float>(-wh_2.y, wh_2.y) };
				q.SetPosition(pos).Draw();
			}
		});
		SetFrame(Frame::Create(tex));
	}
	// todo: blood effect frame 1-8 draw to tex
};
