#include "pch.h"
#include "res_frames.h"

xx::Task<> ResFrames::AsyncLoad(std::string picRoot, int32_t texSiz) {
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;
	co_return co_await AsyncLoad(ffs, picRoot, texSiz);
}

xx::Task<> ResFrames::AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz) {
	ffs.emplace_back(picRoot + "number_outlined_0.png", &number_outlined_0);
	ffs.emplace_back(picRoot + "number_outlined_1.png", &number_outlined_1);
	ffs.emplace_back(picRoot + "number_outlined_2.png", &number_outlined_2);
	ffs.emplace_back(picRoot + "number_outlined_3.png", &number_outlined_3);
	ffs.emplace_back(picRoot + "number_outlined_4.png", &number_outlined_4);
	ffs.emplace_back(picRoot + "number_outlined_5.png", &number_outlined_5);
	ffs.emplace_back(picRoot + "number_outlined_6.png", &number_outlined_6);
	ffs.emplace_back(picRoot + "number_outlined_7.png", &number_outlined_7);
	ffs.emplace_back(picRoot + "number_outlined_8.png", &number_outlined_8);
	ffs.emplace_back(picRoot + "number_outlined_9.png", &number_outlined_9);
	ffs.emplace_back(picRoot + "td_cell_empty.png", &td_cell_empty);
	ffs.emplace_back(picRoot + "td_cell_gold_mine.png", &td_cell_gold_mine);
	ffs.emplace_back(picRoot + "td_cell_montain.png", &td_cell_montain);
	ffs.emplace_back(picRoot + "td_cell_mouse_focus.png", &td_cell_mouse_focus);
	ffs.emplace_back(picRoot + "td_cell_road.png", &td_cell_road);
	ffs.emplace_back(picRoot + "td_cell_space.png", &td_cell_space);
	ffs.emplace_back(picRoot + "td_cell_water.png", &td_cell_water);
	ffs.emplace_back(picRoot + "td_effect_1.png", &td_effect_1);
	ffs.emplace_back(picRoot + "td_icon_arrow1.png", &td_icon_arrow1);
	ffs.emplace_back(picRoot + "td_icon_arrow3.png", &td_icon_arrow3);
	ffs.emplace_back(picRoot + "td_icon_cannon1.png", &td_icon_cannon1);
	ffs.emplace_back(picRoot + "td_icon_cannon3.png", &td_icon_cannon3);
	ffs.emplace_back(picRoot + "td_icon_coin.png", &td_icon_coin);
	ffs.emplace_back(picRoot + "td_icon_sun.png", &td_icon_sun);
	ffs.emplace_back(picRoot + "td_shape_block.png", &td_shape_block);
	ffs.emplace_back(picRoot + "td_shape_box.png", &td_shape_box);
	ffs.emplace_back(picRoot + "td_shape_circle.png", &td_shape_circle);
	ffs.emplace_back(picRoot + "td_shape_dot.png", &td_shape_dot);
	ffs.emplace_back(picRoot + "td_shape_mask.png", &td_shape_mask);
	ffs.emplace_back(picRoot + "td_shape_rect.png", &td_shape_rect);
	ffs.emplace_back(picRoot + "td_shape_ring.png", &td_shape_ring);
	ffs.emplace_back(picRoot + "td_shape_trangle.png", &td_shape_trangle);
	ffs.emplace_back(picRoot + "td_tower_base.png", &td_tower_base);
	ffs.emplace_back(picRoot + "td_ui_aim.png", &td_ui_aim);
	ffs.emplace_back(picRoot + "td_ui_border.png", &td_ui_border);
	ffs.emplace_back(picRoot + "td_ui_clock.png", &td_ui_clock);
	ffs.emplace_back(picRoot + "td_ui_coin.png", &td_ui_coin);
	ffs.emplace_back(picRoot + "td_ui_gear.png", &td_ui_gear);
	ffs.emplace_back(picRoot + "td_ui_gem.png", &td_ui_gem);
	ffs.emplace_back(picRoot + "td_ui_heart.png", &td_ui_heart);
	ffs.emplace_back(picRoot + "td_ui_menu.png", &td_ui_menu);
	ffs.emplace_back(picRoot + "td_ui_pause.png", &td_ui_pause);
	ffs.emplace_back(picRoot + "td_ui_run1.png", &td_ui_run1);
	ffs.emplace_back(picRoot + "td_ui_run2.png", &td_ui_run2);
	ffs.emplace_back(picRoot + "td_ui_run3.png", &td_ui_run3);
	ffs.emplace_back(picRoot + "td_ui_star.png", &td_ui_star);
	ffs.emplace_back(picRoot + "td_ui_sword.png", &td_ui_sword);

	// load / download
#ifdef __EMSCRIPTEN__
	int32_t downloadCount{};
#endif
	for (auto& ff : ffs) {
#ifdef __EMSCRIPTEN__
		EngineBase3::Instance().tasks.Add([pff = &ff, &downloadCount, this]()->xx::Task<> {
			auto& ff = *pff;
			*ff.second = co_await EngineBase3::Instance().AsyncLoadFrameFromUrl(ff.first);
			++downloadCount;
		});
#else
		*ff.second = EngineBase3::Instance().LoadFrame(ff.first);
#endif
	}
#ifdef __EMSCRIPTEN__
	while (downloadCount < ffs.size()) co_yield 0;
#endif

	// batch combine textures
	if (texSiz) {
		auto ok = DynamicTexturePacker<>::Pack(ffs, texSiz);
		assert(ok);
	}

	// fill groups

	number_outlined_.Add(number_outlined_0);
	number_outlined_.Add(number_outlined_1);
	number_outlined_.Add(number_outlined_2);
	number_outlined_.Add(number_outlined_3);
	number_outlined_.Add(number_outlined_4);
	number_outlined_.Add(number_outlined_5);
	number_outlined_.Add(number_outlined_6);
	number_outlined_.Add(number_outlined_7);
	number_outlined_.Add(number_outlined_8);
	number_outlined_.Add(number_outlined_9);
	td_effect_.Add(td_effect_1);
	co_return;
}
