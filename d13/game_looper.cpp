#include <pch.h>
#include <game_looper.h>

xx::Task<> GameLooper::MainTask() {

#ifdef __EMSCRIPTEN__
	auto tp = co_await AsyncLoadTexturePackerFromUrl("res/d20.blist");
	frame_button = Frame::Create(co_await AsyncLoadTextureFromUrl("res/button.png"));
#else
	auto tp = LoadTexturePacker("res/d20.blist");
	frame_button = LoadFrame("res/button.png");
#endif
	tp->GetToByPrefix(frames_gem, "gem_2_");
	tp->GetToByPrefix(frames_mine, "mine_");
	tp->GetToByPrefix(frames_potion, "potion_4_");

	items.Emplace().Emplace<A>()->Init();
	items.Emplace().Emplace<B>()->Init();
	items.Emplace().Emplace<C>()->Init();

	gLooper.node.Emplace();

	ready = true;
	co_return;
}

void GameLooper::Update() {
	if (!ready) return;
}

void GameLooper::Draw() {
	if (!ready) return;

	// handle mouse over event: draw info panel
	bool over{};
	for (auto& item : items) {
		if (Calc::Intersects::CircleCircle<float>(item->pos.x, item->pos.y, item->radius, mouse.pos.x, mouse.pos.y, 2)) {
			if (!mouseOverItem || mouseOverItem.GetPointer() != item.pointer) {
				mouseOverItem = item.ToWeak();
				node->children.Clear();
				item->FillRichLabel();
			}
			node->position = mouse.pos;
			if (auto d = node->size.y - (windowSize_2.y - mouse.pos.y); d > 0) {
				node->position.y -= d;
			}
			if (auto d = node->size.x - (windowSize_2.x - mouse.pos.x); d > 0) {
				node->position.x -= d;
			}
			node->FillTransRecursive();
			over = true;
			break;
		}
	}
	if (!over) {
		mouseOverItem.Reset();
		node->children.Clear();
	}


	for (auto& o : items) {
		o->Draw();
	}
	DrawNode(node);
}



void A::Init() {
	pos = { -300, 0 };
	radius = 100;
}

void A::FillRichLabel() {
	XY scale{ 1,1 };
	XY padding{ 30, 20 };
	gLooper.node->Init(0, {}, { 1,1 }, { 0,0 });
	auto& rl = gLooper.node->MakeChildren<RichLabel>()->Init(2, padding / 2, scale, {}, 500)
		.SetHAlign(HAligns::Center).AddText(U"halign center\n", { 2,2 })
		.SetHAlign(HAligns::Right).AddText(U"halign right\n", { 0.5, 0.5 })
		.SetHAlign()
		.AddPicture(gLooper.frames_gem[0]).AddText(U" this is gem0. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[1]).AddText(U" this is gem1. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[2]).AddText(U" this is gem2. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[3]).AddText(U" this is gem3. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[4]).AddText(U" this is gem4. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[5]).AddText(U" this is gem5. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[6]).AddText(U" this is gem6. aksd fhkhsd kfhas dfjd kfljslf.\n")
		;
	rl.Commit();
	auto siz = rl.size + padding;
	gLooper.node->MakeChildren<Scale9Sprite>()->Init(1, {}, scale, {}
		, siz, { gLooper.frame_button, {2,2}, {2,3,2,2}, { 0x5f, 0x15, 0xd9, 0xff } });
	gLooper.node->size = siz * scale;
}

void A::Draw() {
	LineStrip().FillCirclePoints({}, radius).SetPosition(pos).SetColor(RGBA8_Red).Draw();
}

void B::Init() {
	pos = { 0, 0 };
	radius = 120;
}

void B::FillRichLabel() {
	XY scale{ 1,1 };
	XY padding{ 30, 20 };
	gLooper.node->Init(0, {}, { 1,1 }, { 0,0 });
	auto& rl = gLooper.node->MakeChildren<RichLabel>()->Init(2, padding / 2, scale, {}, 500)
		.SetHAlign(HAligns::Center).AddText(U"halign center\n", { 2,2 })
		.SetHAlign(HAligns::Right).AddText(U"halign right\n", { 0.5, 0.5 })
		.SetHAlign()
		.AddPicture(gLooper.frames_gem[0]).AddText(U" this is gem0. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[1]).AddText(U" this is gem1. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[2]).AddText(U" this is gem2. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[3]).AddText(U" this is gem3. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[4]).AddText(U" this is gem4. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[5]).AddText(U" this is gem5. aksd fhkhsd kfhas dfjd kfljslf.\n")
		.AddPicture(gLooper.frames_gem[6]).AddText(U" this is gem6. aksd fhkhsd kfhas dfjd kfljslf.\n")
		;
	rl.Commit();
	auto siz = rl.size + padding;
	gLooper.node->MakeChildren<Scale9Sprite>()->Init(1, {}, scale, {}
	, siz, { gLooper.frame_button, {2,2}, {2,3,2,2}, { 127, 127, 127, 255 } });
	gLooper.node->size = siz * scale;
}

void B::Draw() {
	LineStrip().FillCirclePoints({}, radius).SetPosition(pos).SetColor(RGBA8_Green).Draw();
}

void C::Init() {
	pos = { 300, 0 };
	radius = 140;
}

void C::FillRichLabel() {
	XY padding{ 30, 20 };
	gLooper.node->Init(0, {}, { 1,1 }, { 0,0 });
	auto& rl = gLooper.node->MakeChildren<RichLabel>()->Init(2, padding / 2, { 1,1 }, {}, 300)

		.AddPicture(gLooper.frames_gem[6], {2,2})
		.SetOffset(70).AddText("KING OF HELM", { 1,1 }, { 0xfa, 0x14, 0xb4, 0xff }, VAligns::Top)
		.SetOffset(70).AddText("UNIQUE\n", { 1,1 }, { 0xd2, 0x82, 0x3c, 0xff }, VAligns::Bottom)

		.AddText("LEVEL: 100\n", { 1,1 }, {0xff, 0xf2, 0xd2, 0xff})


		.AddPicture(gLooper.frames_potion[0], { 1,1 })
		.AddText("+1,700", { 1,1 }, {0xd2, 0x82, 0x3c, 0xff})
		.AddText(" (+162) ", { 1,1 }, {0x24, 0xc8, 0x24, 0xff})
		.AddText("HP\n", { 1,1 }, { 0xd2, 0x82, 0x3c, 0xff })

		.AddPicture(gLooper.frames_potion[0], { 1,1 })
		.AddText("+170", { 1,1 }, {0xd2, 0x82, 0x3c, 0xff})
		.AddText(" (-52) ", { 1,1 }, {0xc8, 0x0f, 0x0f, 0xff})
		.AddText("MANA\n", { 1,1 }, { 0xd2, 0x82, 0x3c, 0xff })

		.AddPicture(gLooper.frames_potion[0], { 1,1 })
		.AddText("+510", { 1,1 }, {0xd2, 0x82, 0x3c, 0xff})
		.AddText(" (-154) ", { 1,1 }, {0xc8, 0x0f, 0x0f, 0xff})
		.AddText("DMG\n", { 1,1 }, { 0xd2, 0x82, 0x3c, 0xff })

		.AddPicture(gLooper.frames_potion[0], { 1,1 })
		.AddText("+5% ", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddText("CRITICAL CHANCE\n", { 1,1 }, { 0xff, 0xa0, 0x40, 0xff })


		.AddPicture(gLooper.frames_mine[0], { 1,1 })
		.AddText("+808 ", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddText("HP\n", { 1,1 }, { 0xff, 0xa0, 0x40, 0xff })

		.AddPicture(gLooper.frames_mine[0], { 1,1 })
		.AddText("+8% ", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddText("LIGHTNING DMG\n", { 1,1 }, { 0xff, 0xf2, 0xd2, 0xff })

		.AddPicture(gLooper.frames_mine[0], { 1,1 })
		.AddText("+14% ", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddText("CRITICAL DMG\n", { 1,1 }, { 0xff, 0xf2, 0xd2, 0xff })

		.AddPicture(gLooper.frames_mine[0], { 1,1 })
		.AddText("+7% ", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddText("COLD DMG\n", { 1,1 }, { 0xff, 0xf2, 0xd2, 0xff })


		.AddText("PRICE", { 1,1 }, {0x00, 0xbe, 0xff, 0xff})
		.AddPicture(gLooper.frames_gem[6], { 1,1 })
		.AddText("23,768", { 1,1 }, { 0xff, 0x00, 0xff, 0xff })

		;
	rl.Commit();
	auto siz = rl.size + padding;
	gLooper.node->MakeChildren<Scale9Sprite>()->Init(1, {}, {1, 1}, {}
	, siz, { gLooper.frame_button, {2,2}, {2,3,2,2}, { 20, 20, 80, 255 } });
	gLooper.node->size = siz;
}

void C::Draw() {
	LineStrip().FillCirclePoints({}, radius).SetPosition(pos).SetColor(RGBA8_Blue).Draw();
}
