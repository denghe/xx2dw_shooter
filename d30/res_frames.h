#include <engine.h>

struct ResFrames {
	xx::Task<> AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz = 2048);
	xx::Task<> AsyncLoad(std::string picRoot, int32_t texSiz = 2048);


	xx::Ref<Frame> number_outlined_0;
	xx::Ref<Frame> number_outlined_1;
	xx::Ref<Frame> number_outlined_2;
	xx::Ref<Frame> number_outlined_3;
	xx::Ref<Frame> number_outlined_4;
	xx::Ref<Frame> number_outlined_5;
	xx::Ref<Frame> number_outlined_6;
	xx::Ref<Frame> number_outlined_7;
	xx::Ref<Frame> number_outlined_8;
	xx::Ref<Frame> number_outlined_9;
	xx::Ref<Frame> td_cell_empty;
	xx::Ref<Frame> td_cell_gold_mine;
	xx::Ref<Frame> td_cell_montain;
	xx::Ref<Frame> td_cell_mouse_focus;
	xx::Ref<Frame> td_cell_road;
	xx::Ref<Frame> td_cell_space;
	xx::Ref<Frame> td_cell_water;
	xx::Ref<Frame> td_effect_1;
	xx::Ref<Frame> td_icon_arrow1;
	xx::Ref<Frame> td_icon_arrow3;
	xx::Ref<Frame> td_icon_cannon1;
	xx::Ref<Frame> td_icon_cannon3;
	xx::Ref<Frame> td_icon_coin;
	xx::Ref<Frame> td_icon_sun;
	xx::Ref<Frame> td_shape_block;
	xx::Ref<Frame> td_shape_box;
	xx::Ref<Frame> td_shape_circle;
	xx::Ref<Frame> td_shape_dot;
	xx::Ref<Frame> td_shape_mask;
	xx::Ref<Frame> td_shape_rect;
	xx::Ref<Frame> td_shape_ring;
	xx::Ref<Frame> td_shape_trangle;
	xx::Ref<Frame> td_tower_base;
	xx::Ref<Frame> td_ui_aim;
	xx::Ref<Frame> td_ui_border;
	xx::Ref<Frame> td_ui_clock;
	xx::Ref<Frame> td_ui_coin;
	xx::Ref<Frame> td_ui_gear;
	xx::Ref<Frame> td_ui_gem;
	xx::Ref<Frame> td_ui_heart;
	xx::Ref<Frame> td_ui_menu;
	xx::Ref<Frame> td_ui_pause;
	xx::Ref<Frame> td_ui_run1;
	xx::Ref<Frame> td_ui_run2;
	xx::Ref<Frame> td_ui_run3;
	xx::Ref<Frame> td_ui_star;
	xx::Ref<Frame> td_ui_sword;

	xx::Listi32<xx::Ref<Frame>> number_outlined_;
	xx::Listi32<xx::Ref<Frame>> td_effect_;
};
