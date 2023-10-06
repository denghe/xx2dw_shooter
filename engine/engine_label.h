#pragma once
#include "engine_node.h"

struct Label : Node {
	xx::List<TinyFrame const*, int32_t> fs;
	RGBA8 color;

	void Init(int z_, XY const& position_, XY const& anchor_, RGBA8 color_, std::u32string_view const& txt_) {
		z = z_;
		position = position_;
		anchor = anchor_;
		color = color_;
		auto len = txt_.size();
		fs.Resize(len);
		auto& ctc = EngineBase2::Instance().ctcDefault;
		size = { 0, (float)ctc.canvasHeight };
		for (size_t i = 0; i < len; ++i) {
			fs[i] = &ctc.Find(txt_[i]);
			size.x += fs[i]->texRect.w;
		}
		FillTrans();
	}

	virtual void Draw() override {
		auto& shader = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
		auto pos = trans;
		for (auto& f : fs) {
			auto& q = *shader.Draw(f->tex->GetValue(), 1);
			q.anchor = { 0.f, 0.f };
			q.color = color;
			q.colormulti = 1;
			q.pos = pos;
			q.radians = {};
			q.scale = { 1, 1 };
			q.texRect.data = f->texRect.data;
			pos.x += f->texRect.w;
		}
	}
};
