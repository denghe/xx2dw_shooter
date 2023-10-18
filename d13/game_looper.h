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

	enum class ItemTypes {
		Unknown, Space, Text, Picture	// ... more
	};

	struct ItemBase {
		ItemTypes type;
		VAligns align;
		float x, y, h;	// changed by  FillXH , FillY
	};

	struct Space : ItemBase {
		XY size;
		Space(VAligns align, XY const& size) {
			this->type = ItemTypes::Space;
			this->align = align;
			this->size = size;
		}
	};

	struct Text : ItemBase {
		RGBA8 color;
		xx::List<TinyFrame const*, int32_t> chars;
		Text(VAligns align, RGBA8 color) {
			this->type = ItemTypes::Text;
			this->align = align;
			this->color = color;
		}
	};

	struct Picture : ItemBase {
		xx::Shared<Quad> quad;
		Picture(VAligns align, xx::Shared<Quad> quad) {
			this->type = ItemTypes::Picture;
			this->align = align;
			this->quad = std::move(quad);
		}
	};

	// ... more

	struct Item {
		std::aligned_storage_t<xx::MaxSizeof_v<Space, Text, Picture>, sizeof(size_t)> data;

		Item(Item const&) = delete;
		Item& operator=(Item const&) = delete;
		Item& operator=(Item&& o) noexcept = delete;
		Item(Item&& o) noexcept {
			memcpy(&data, &o.data, sizeof(data));
			o.Type() = ItemTypes::Unknown;
		}
		Item() {
			((ItemBase&)data).type = ItemTypes::Unknown;
		}

		template<std::derived_from<ItemBase> T, typename ... Args>
		T& Emplace(Args&& ...args) {
			assert(((ItemBase&)data).type == ItemTypes::Unknown);
			return *new (&data) T(std::forward<Args>(args)...);
		}

		template<std::derived_from<ItemBase> T>
		T& As() {
			auto& ib = (ItemBase&)data;
			if constexpr (std::same_as<T, Space>) assert(ib.type == ItemTypes::Space);
			if constexpr (std::same_as<T, Text>) assert(ib.type == ItemTypes::Text);
			if constexpr (std::same_as<T, Picture>) assert(ib.type == ItemTypes::Picture);
			return (T&)ib;
		}

		ItemTypes const& Type() const {
			return ((ItemBase&)data).type;
		}
		ItemTypes& Type() {
			return ((ItemBase&)data).type;
		}

		~Item() {
			auto& ib = (ItemBase&)data;
			switch (ib.type) {
			case ItemTypes::Unknown:
				break;
			case ItemTypes::Space:
				((Space&)ib).~Space();
				break;
			case ItemTypes::Text:
				((Text&)ib).~Text();
				break;
			case ItemTypes::Picture:
				((Picture&)ib).~Picture();
				break;
			default: xx_assert(false);
			}
		}
	};

	xx::List<Item, int32_t> items;
	xx::List<Picture*, int32_t> pics;				// for batch draw
	float width{}, y{}, lineX{}, lineHeight{};
	int32_t lineItemsCount{};

	RichNode& Init(int z_, XY const& position_, XY const& scale_, XY const& anchor_, float width_, int capacity = 128) {
		assert(width_ > 0);
		z = z_;
		position = position_;
		anchor = anchor_;
		scale = scale_;
		width = width_;
		y = {};

		LineInit();
		items.Clear();
		items.Reserve(capacity);
		return *this;
	}

protected:

	template<std::derived_from<ItemBase> T, typename ... Args>
	T& AddItem(Args&& ...args) {
		++lineItemsCount;
		return items.Emplace().Emplace<T>(std::forward<Args>(args)...);
	}

	void FillXH(ItemBase& o, float h) {
		o.x = lineX;
		o.h = h;
		if (lineHeight < h) {
			lineHeight = h;
		}
	}

	Text& MakeText(VAligns align, RGBA8 color) {
		auto& o = AddItem<Text>(align, color);
		FillXH(o, (float)EngineBase2::Instance().ctcDefault.canvasHeight);
		return o;
	}

	Text& GetLastText(VAligns align, RGBA8 color) {
		if (items.Empty() || items.Back().Type() != ItemTypes::Text) return MakeText(align, color);
		if (auto& t = items.Back().As<Text>(); t.align == align && t.color == color) return t;
		return MakeText(align, color);
	}

	void FillY() {
		y -= lineHeight;
		for (int e = items.len, i = e - lineItemsCount; i < e; ++i) {
			auto& o = items[i].As<ItemBase>();
			float leftHeight = lineHeight - o.h;
			assert(leftHeight >= 0);
			if (o.align == VAligns::Bottom || std::abs(leftHeight) < std::numeric_limits<float>::epsilon()) {
				o.y = y;
			} else if (o.align == VAligns::Top) {
				o.y = y + leftHeight;
			} else {	// o.align == VAligns::Center
				o.y = y + leftHeight / 2;
			}
		}
	}

	void LineInit() {
		lineX = {};
		lineHeight = {};
		lineItemsCount = {};
	}

	void NewLine() {
		FillY();
		LineInit();
	}

public:

	RichNode& AddText(std::u32string_view const& text, RGBA8 color = RGBA8_White, VAligns align = VAligns::Center) {
		auto& ctc = EngineBase2::Instance().ctcDefault;
		Text* t{};
		TinyFrame* charFrame{};
		float charWidth, leftWidth{};
		for (auto& c : text) {
			if (c == '\r') continue;
			else if (c == '\n') {
				NewLine();
				MakeText(align, color);
			} else {
				charFrame = &ctc.Find(c);
				charWidth = charFrame->texRect.w;
				leftWidth = width - lineX;
				if (leftWidth >= charWidth) {
					t = &GetLastText(align, color);
				} else {
					NewLine();
					t = &MakeText(align, color);
				}
				t->chars.Add(charFrame);
				lineX += charWidth;
			}
		}
		return *this;
	}

	RichNode& AddPicture(xx::Shared<Quad> quad, VAligns align = VAligns::Center) {
		assert(quad);
		quad->anchor = {};
		auto size = quad->Size();	// scale ?
		auto leftWidth = width - lineX;
		if (leftWidth < size.x) {
			NewLine();
		}
		auto& o = AddItem<Picture>(align, std::move(quad));
		FillXH(o, size.y);
		lineX += size.x;
		return *this;
	}

	RichNode& AddPicture(xx::Shared<Frame> frame, VAligns align = VAligns::Center) {
		auto quad = xx::Make<Quad>();
		quad->SetFrame(std::move(frame));
		return AddPicture(std::move(quad), align);
	}

	RichNode& AddSpace(XY const& size, VAligns align = VAligns::Center) {
		auto leftWidth = width - lineX;
		if (leftWidth < size.x) {
			NewLine();
		}
		auto& o = AddItem<Space>(align, size);
		FillXH(o, size.y);
		lineX += size.x;
		return *this;
	}

	RichNode& AddSpace(float width) {
		return AddSpace({ width,0 });
	}


	// ... more

	RichNode& Commit() {
		FillY();
		size = { width, -y };
		FillTrans();
		return *this;
	}

	virtual void Draw() override {
		auto& shader = EngineBase1::Instance().ShaderBegin(EngineBase1::Instance().shaderQuadInstance);
		XY basePos = trans;
		basePos.y -= y;
		pics.Clear();
		for (auto& o : items) {
			switch (o.Type()) {
			case ItemTypes::Space: {
				auto& t = o.As<Space>();
				// todo
				break;
			}
			case ItemTypes::Text: {
				auto& t = o.As<Text>();
				auto pos = basePos + XY{ t.x, t.y };
				for (auto& f : t.chars) {
					auto& q = *shader.Draw(f->tex->GetValue(), 1);
					q.anchor = { 0.f, 0.f };
					q.color = t.color;
					q.colorplus = 1;
					q.pos = pos;
					q.radians = {};
					q.scale = { trans.a, trans.d };
					q.texRect.data = f->texRect.data;
					pos.x += f->texRect.w * trans.a;
				}
				break;
			}
			case ItemTypes::Picture: {
				auto& t = o.As<Picture>();
				pics.Add(&t);
				break;
			}
			default: xx_assert(false);
			}
		}
		for (auto& p : pics) {
			auto& t = *p;
			auto pos = basePos + XY{ t.x, t.y };
			t.quad->SetPosition(pos).Draw();
		}
	}
};
