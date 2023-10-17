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


// todo: store std::u32string content's tiny container

struct U32StringStore {
	void* buf;

};


//struct DataShared {
//
//	struct Header {
//		size_t len;
//		size_t numRefs;
//	};
//	static constexpr size_t hLen = Data::GetBufHeaderReserveLen();
//	static_assert(hLen >= sizeof(Header));
//
//	Header* h;
//
//	// 供 if 简单判断是否为空
//	XX_INLINE operator bool() const {
//		return h != nullptr;
//	}
//
//	XX_INLINE DataShared() : h(nullptr) {}
//
//	XX_INLINE DataShared(Data&& d) {
//		if (d) {
//			h = (Header*)(d.buf - hLen);
//			h->len = d.len;
//			h->numRefs = 1;
//			d.Reset();
//		} else {
//			h = nullptr;
//		}
//	}
//
//	XX_INLINE DataShared(DataShared const& ds)
//		: h(ds.h) {
//		if (h) {
//			assert(h->numRefs > 0);
//			++h->numRefs;
//		}
//	}
//
//	DataShared(DataShared&& ds) noexcept
//		: h(ds.h) {
//		ds.h = nullptr;
//	}
//
//	XX_INLINE DataShared& operator=(DataShared const& ds) {
//		if (h == ds.h) return *this;
//		Clear();
//		h = ds.h;
//		if (h) {
//			assert(h->numRefs > 0);
//			++h->numRefs;
//		}
//		return *this;
//	}
//
//	XX_INLINE DataShared& operator=(DataShared&& ds) noexcept {
//		std::swap(h, ds.h);
//		return *this;
//	}
//
//	XX_INLINE void Clear() {
//		if (h) {
//			if (--h->numRefs == 0) {
//				free(h);
//			}
//			h = nullptr;
//		}
//	}
//
//	~DataShared() {
//		Clear();
//	}
//
//	XX_INLINE uint8_t* GetBuf() const {
//		if (h) return (uint8_t*)h + hLen;
//		else return nullptr;
//	}
//
//	XX_INLINE size_t GetLen() const {
//		if (h) return h->len;
//		else return 0;
//	}
//
//	XX_INLINE size_t GetNumRefs() const {
//		if (h) return h->numRefs;
//		else return 0;
//	}
//};
//
//// mem moveable tag
//template<>
//struct IsPod<DataShared, void> : std::true_type {};
//}



enum class VAligns {
	Top,
	Center,
	Bottom
};

struct RichNode : Node {

	struct Text {
		VAligns align;
		RGBA8 color;
		std::unique_ptr<std::u32string> text;
	};

	struct Picture {
		VAligns align;
		xx::Shared<Quad> quad;
	};

	struct Block {
		VAligns align;
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

	RichNode& SetOffset(XY offset) {
		items.emplace_back(offset);
		return *this;
	}

	RichNode& AddText(VAligns align, RGBA8 color, std::u32string_view const& text) {
		items.emplace_back(Text{ align, color, std::make_unique<std::u32string>(text) });
		return *this;
	}

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
