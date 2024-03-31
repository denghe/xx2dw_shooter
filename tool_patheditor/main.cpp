#include <pch.h>

struct GameLooper;
inline GameLooper* gLooper{};
struct GameLooper : Engine<GameLooper>, GDesign<1280, 800, 60> {
	xx::Task<> DrawTask() {
		
		// todo
		running = false;
		co_return;
	}
};

int main(int argc, char** argv) {
	// todo: handle args & store to g
	{
		gLooper = new GameLooper();
		auto sg = xx::MakeScopeGuard([&] { delete gLooper; gLooper = {}; });
		gLooper->showFps = true;
		//gLooper->showWindow = false;	// todo
		gLooper->title = "xx2dw_tool_patheditor";
		gLooper->Init();
		gLooper->Run<true>();		// break when running == false
	}
	// todo: print info
	xx::CoutN("done. press any to continue...");
	//std::cin.get();

	return 0;
}
