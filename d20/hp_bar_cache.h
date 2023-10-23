#pragma once
#include <game_looper.h>

/*
   template<int32_t width_ = 100, int32_t height_ = 7>
   struct HpBarCache;
*/

template<int32_t width_, int32_t height_>
struct HpBarCache : DynamicTexturePacker<> {
	static constexpr XY cPadding{ 4, 4 };
	static constexpr int32_t cWidth{ width_ };
	static constexpr float cWidth_2{ (float)width_ / 2 };
	static constexpr int32_t cHeight{ height_ };
	static constexpr float cHeight_2{ (float)height_ / 2 };

	// index == hp value
	std::vector<xx::Ref<Frame>> fs;

	// all hp value bar -> textures -> frames -> dynamic pack
	void Init(Scale9SpriteConfig const& cfg) {

		FrameBuffer fb;
		fb.Init();

		Quad q;				// hp
		q.SetFrame(gLooper.frame_dot_1_22).SetColor({ 255,0,0,255 }).SetAnchor({ 0, 0.5 }).SetPosition({ -cWidth_2, 0 }); // 204 ?

		Scale9Sprite s9;	// bg
		s9.Init(0, {}, { 0.5, 0.5 }, cfg.GetCornerSize() + XY{ (float)cWidth, cHeight }, cfg);
		s9.FillTrans();

		fs.clear();
		for (int i = 0; i <= cWidth; ++i) {
			auto tex = FrameBuffer::MakeTexture({ uint32_t(cWidth + cPadding.x), uint32_t(cHeight + cPadding.y) });
			fb.DrawTo(tex, {}, [&] {
				q.SetScale({ i / 2.f, cHeight_2 }).Draw();
				s9.Draw();
			});
			fs.emplace_back(Frame::Create(tex));
		}

		xx_assert(Fill(fs));
	}
};
