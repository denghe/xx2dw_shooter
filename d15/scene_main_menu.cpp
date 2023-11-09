#include <pch.h>
#include <all.h>

struct InfoPanel : Node {
	constexpr static RGBA8 cColorPink{ 255, 0, 255, 255 };
	constexpr static RGBA8 cColorPinkDark{ 250, 20, 180, 255 };
	constexpr static RGBA8 cColorOrange{ 255, 160, 64, 255 };
	constexpr static RGBA8 cColorOrangeDark{ 210, 130, 60, 255 };
	constexpr static RGBA8 cColorGreen{ 36, 200, 36, 255 };
	constexpr static RGBA8 cColorRed{ 200, 15, 15, 255 };
	constexpr static RGBA8 cColorWhite{ 255, 242, 210, 255 };
	constexpr static RGBA8 cColorWhiteDark{ 218, 207, 179, 255 };

	constexpr static RGBA8 cBorderColor{ cColorWhiteDark };
	constexpr static RGBA8 cBackgroundColor{ RGBA8_Black };

	constexpr static float cPadding{ 5.f }, cBorderWidth{ 2.f };

	xx::Shared<RichLabel> content;
	Quad border;

	// todo: copy code from button
	// todo: rich label init code use lambda
	template<typename Func>
	void Init(int z_, XY const& position_, XY const& scale_, XY const& anchor_, float width_, Func&& fillContent) {
		assert(width_ > 0);
		Node::Init(z_, position_, scale_, anchor_, {width_, 1});
		children.Clear();
		content.Emplace()->Init(z_ + 1, position_, scale_, anchor_, width_);
		children.Add(content);
		border.SetFrame(gLooper.frame_dot_1_22);

		fillContent(content);
		size = content->size + cPadding;

		// bg? border?

		FillTransRecursive();
	}

	// todo: draw border line + bg like scale 9 sprite
	virtual void Draw() override {
		//// todo: calculate scale basePos
		// auto contentSize = content->size;
		// auto borderSize1 = contentSize + XY{ cPadding * 2, cPadding * 2 };
		// auto borderSize2 = borderSize1 + XY{ cBorderWidth * 2, cBorderWidth * 2 };
		// // todo: draw border line + bg for alpha
		// //border.SetPosition()
		// border.SetScale(border.texRect.w / borderSize2.x).SetColor(cBorderColor).Draw();
		// border.SetScale(border.texRect.w / borderSize1.x).SetColor(cBackgroundColor).Draw();
		// 
		//// todo: draw border
		LineStrip().FillRectPoints(worldMinXY, worldSize).Draw();
	}
};

void SceneMainMenu::Init() {
	rootNode.Emplace()->Init();

	auto&& ip = rootNode->MakeChildren<InfoPanel>();
	ip->Init(0, {}, { 1,1 }, { 0.5f, 0.5f }, 300, [](xx::Shared<RichLabel>& content) {
		content->AddText("asdf").Commit();
	});
}

void SceneMainMenu::Draw() {
	gLooper.DrawNode(rootNode);
};
