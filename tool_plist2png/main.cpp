#include <pch.h>
#include <engine_texturepacker_reader.h>

// todo: edit for store frame buffer
void saveImage(char* filepath, GLFWwindow* w) {
	int width, height;
	glfwGetFramebufferSize(w, &width, &height);
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	GLsizei bufferSize = stride * height;
	std::vector<char> buffer(bufferSize);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}

constexpr GDesign<1280, 800, 60> gDesign;

struct GameLooper;
GameLooper* gLooper;
struct GameLooper : Engine<GameLooper>, decltype(gDesign) {
	xx::Task<> MainTask() {
		// todo: file path from args

		auto [fd, fn] = LoadFileData("C:/Codes/KingdomRushFrontiers/Resources/towers-hd.plist"sv);

		TexturePackerReader::Plist tp;
		if (int r = tp.Load(fd)) {
			std::cerr << "ReadAllBytes failed. r = " << r << " fn = " << fn << std::endl;
			running = false;
			co_return;
		}

		std::string rootPath;
		if (auto&& i = fn.find_last_of("/"); i != fn.npos) {
			rootPath = fn.substr(0, i + 1);
		}
		auto t = LoadTexture(rootPath + tp.metadata.textureFileName);

		FrameBuffer fb{ true };

		for (auto& fi : tp.frames) {
			auto f = Frame::Create(t);
			//f->anchor = ...
			//f->anchor = ...
			//f->anchor = ...

			//fb.Draw()

			for (auto& a : fi.aliases) {
				// todo: write same data to filename: a
			}
		}


		running = false;
		co_return;
	}
};

int main(int argc, char** argv) {
	// todo: handle args & store to gLooper

	// popup gl window to draw & store pics to disk
	{
		gLooper = new GameLooper();
		auto sg = xx::MakeScopeGuard([&] { delete gLooper; });
		gLooper->showFps = false;
		//gLooper->showWindow = false;	// todo
		gLooper->title = "xx2dw_tool_plist2png";
		gLooper->Init();
		gLooper->Run<false>();		// break when running == false
	}

	// todo: print info
	xx::CoutN("done. press any to continue...");
	std::cin.get();

	return 0;
}
