#include <engine.h>

struct ResFrames {
	xx::Task<> AsyncLoad(std::vector<std::pair<std::string, xx::Ref<Frame>*>>& ffs, std::string picRoot, int32_t texSiz = 2048);
	xx::Task<> AsyncLoad(std::string picRoot, int32_t texSiz = 2048);


	xx::Ref<Frame> egg_black;
	xx::Ref<Frame> egg_blue;
	xx::Ref<Frame> egg_cyan;
	xx::Ref<Frame> egg_green;
	xx::Ref<Frame> egg_orange;
	xx::Ref<Frame> egg_purple;
	xx::Ref<Frame> egg_red;
	xx::Ref<Frame> egg_white;
	xx::Ref<Frame> egg_yellow;
	xx::Ref<Frame> ui_border;
};
