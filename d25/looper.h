#pragma once
#include <engine.h>

constexpr GDesign<1280, 800, 60> gDesign;

struct Looper : Engine<Looper>, decltype(gDesign) {
	xx::Task<> MainTask();


	xx::Listi32<xx::Ref<Frame>> frames_number_outlined;
	xx::Ref<Frame> frame_td_ui_border;
	xx::Ref<Frame> frame_td_shape_dot;
	xx::Ref<Frame> frame_td_ui_star;
	xx::Ref<Frame> frame_td_ui_gem;
	xx::Ref<Frame> frame_td_ui_gear;
	xx::Ref<Frame> frame_td_ui_coin;
	xx::Ref<Frame> frame_td_ui_menu;
	xx::Ref<Frame> frame_td_ui_pause;
	xx::Ref<Frame> frame_td_ui_run1;
	xx::Ref<Frame> frame_td_ui_run2;
	xx::Ref<Frame> frame_td_ui_run3;
	xx::Ref<Frame> frame_td_ui_clock;
	xx::Ref<Frame> frame_td_ui_aim;
	xx::Ref<Frame> frame_td_ui_heart;
	xx::Ref<Frame> frame_td_ui_sword;
	xx::Ref<Frame> frame_td_cell_gold_mine;
	xx::Ref<Frame> frame_td_icon_sun;
	xx::Ref<Frame> frame_td_cell_water;
	xx::Ref<Frame> frame_td_icon_coin;
	xx::Ref<Frame> frame_td_cell_road;
	xx::Ref<Frame> frame_td_icon_cannon3;
	xx::Ref<Frame> frame_td_icon_cannon1;
	xx::Ref<Frame> frame_td_icon_arrow3;
	xx::Ref<Frame> frame_td_icon_arrow1;
	xx::Ref<Frame> frame_td_tower_base;
	xx::Ref<Frame> frame_td_cell_mouse_focus;
	xx::Ref<Frame> frame_td_cell_montain;
	xx::Ref<Frame> frame_td_cell_space;
	xx::Ref<Frame> frame_td_cell_empty;
	xx::Ref<Frame> frame_td_shape_trangle;
	xx::Ref<Frame> frame_td_shape_circle;
	std::vector<std::pair<std::string, xx::Ref<Frame>*>> ffs;

	Scale9SpriteConfig s9cfg;

	// tiled map & frames
	xx::Ref<TMX::Map> map_stage1, map_stage2;

	// for grid  FindNearest  ForeachByRange
	SpaceGridRingDiffuseData sgrdd;


	xx::Ref<TMX::Map> map1, map2;	// todo: remove these code
};

extern Looper gLooper;
