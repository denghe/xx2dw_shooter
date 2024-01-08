#pragma once
#include <game_looper.h>

/*
   template<int32_t width_ = 64, int32_t height_ = 7>
   struct HpBarCache;
*/

template<uint32_t width_, uint32_t height_>
struct HpBarCache : protected DynamicTexturePacker<> {
	static constexpr Vec2<uint32_t> cPadding{ 1, 1 };	// border width
	static constexpr Vec2<uint32_t> cSpacing{ 1, 1 };

	static constexpr Vec2<uint32_t> cTextureSize{ width_ + cPadding.x + cSpacing.x, height_ + cPadding.y + cSpacing.y };

	static constexpr XY cBackgroundSize{ float(width_ + cPadding.x), float(height_ + cPadding.y) };
	static constexpr XY cBloodSize{ float(width_), float(height_) };

	static constexpr XY cBackgroundPos{ -cBackgroundSize.x / 2, -cBackgroundSize.y / 2 };
	static constexpr XY cBloodPos{ -cBloodSize.x / 2, -cBloodSize.y / 2 };

	static constexpr XY cBackgroundScale{ cBackgroundSize.x / 2, cBackgroundSize.y / 2 };
	static constexpr XY cBloodScale{ cBloodSize.x / 2, cBloodSize.y / 2 };

	static constexpr RGBA8 cBackgroundColor{ RGBA8_Black };
	static constexpr RGBA8 cBloodColor{ RGBA8{204,0,0,255} };

	// index == hp value
	std::vector<xx::Ref<Frame>> fs;

	// all hp value bar -> textures -> frames -> dynamic pack
	void Init(Scale9SpriteConfig const& cfg) {

		FrameBuffer fb;
		fb.Init();

		Quad q;
		q.SetFrame(gLooper.frame_dot_1_22).SetAnchor({});

		fs.clear();
		for (int i = 0; i <= width_; ++i) {
			auto tex = FrameBuffer::MakeTexture(cTextureSize);
			fb.DrawTo(tex, {}, [&] {
				q.SetPosition(cBackgroundPos)
					.SetScale(cBackgroundScale)
					.SetColor(cBackgroundColor)
					.Draw();

				q.SetPosition(cBloodPos)
					.SetScale(cBloodScale * XY{ (float)i / width_, 1 })
					.SetColor(cBloodColor)
					.Draw();
			});
			fs.emplace_back(Frame::Create(tex));
		}

		xx_assert(Fill(fs));	// combine to big texture & release all small textures
	}

	/*
		// example:
		auto p = pos + XY{ 0, texRect.h * scale.y };
		auto&& f = gLooper.hpBarCache->Get(hp, maxHP);
		q.SetFrame(f).SetPosition(p).SetScale({ GetSizeScaled().x / gLooper.hpBarCache->cBloodSize.x , 1 }).Draw();
	*/
	xx::Ref<Frame>& Get(int32_t hp, int32_t maxHP) {
		auto idx = int32_t((float)hp / maxHP * cBloodSize.x);
		return fs[idx];
	}
};
