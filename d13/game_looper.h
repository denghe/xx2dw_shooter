#pragma once
#include <engine.h>

constexpr GDesign<1280, 720, 60> gDesign;

struct RichNode;

struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	void Update();
	xx::Task<> MainTask();
	void Draw();

	xx::Shared<RichNode> rn;
};
extern GameLooper gLooper;

enum class VAligns {
	Top,
	Center,
	Bottom
};

struct RichNode : Node {
	struct Base {
		VAligns align;
	};
	struct Text : Base {
		RGBA8 color;
		xx::List<TinyFrame const*> tfs;
	};
	struct Picture : Base {
		xx::Shared<Quad> quad;
	};
	struct Block : Base {
		RGBA8 color;
		XY size;
	};
	// ... more

	using Item = std::variant<Block, Text, Picture>;
	std::vector<Item> items;
	float width{};

	RichNode& Init(int z_, XY const& position_, XY const& scale_, XY const& anchor_, float width_) {
		z = z_;
		position = position_;
		anchor = anchor_;
		scale = scale_;
		width = width_;
		return *this;
	}

	RichNode& AddPicture(VAligns align, xx::Shared<Quad> const& quad) {
		items.emplace_back(Picture{ align, quad });
		return *this;
	}

	RichNode& AddBlock(VAligns align, RGBA8 color, XY const& size) {
		items.emplace_back(Block{ align, color, size});
		return *this;
	}

	RichNode& AddText(VAligns align, RGBA8 color, std::u32string_view const& text) {
		auto& t = items.emplace_back(Text{ align, color });
		auto& tfs = std::get<Text>(t).tfs;
		tfs.Reserve(text.size());
		for (auto& c : text) {
			// todo: handle \n
			
		}
		return *this;
	}

	// ... more

	RichNode& Commit() {
		// todo: gen cache, calc size, FillTrans();
	//		auto len = (int)txt_.size();
	//		fs.Resize(len);
	//		auto& ctc = EngineBase2::Instance().ctcDefault;
	//		//size = { 0, (float)ctc.canvasHeight };
	//		//for (int i = 0; i < len; ++i) {
	//		//	fs[i] = &ctc.Find(txt_[i]);
	//		//	size.x += fs[i]->texRect.w;
	//		//}
	//		//FillTrans();
	//	}
	//
		return *this;
	}

	virtual void Draw() override {
		// todo
	//		auto& shader = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
	//		XY pos = trans;
	//		for (auto& f : fs) {
	//			auto& q = *shader.Draw(f->tex->GetValue(), 1);
	//			q.anchor = { 0.f, 0.f };
	//			q.color = color;
	//			q.colorplus = 1;
	//			q.pos = pos;
	//			q.radians = {};
	//			q.scale = { trans.a, trans.d };
	//			q.texRect.data = f->texRect.data;
	//			pos.x += f->texRect.w * trans.a;
	//		}
	}
};
