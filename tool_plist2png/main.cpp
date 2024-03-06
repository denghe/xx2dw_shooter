#include <pch.h>
#include <engine_texturepacker_reader.h>

struct GameLooper : Engine<GameLooper>, GDesign<1280, 800, 60> {
	inline static GameLooper* instance;	// init by main()
	xx::Task<> DrawTask() {
		// todo: file path from args
#if 1
		auto [fd, fn] = LoadFileData("C:/Codes/tmp/gen/test.plist"sv);
#else
		auto [fd, fn] = LoadFileData("C:/Codes/tmp/gen2/towers-hd.plist"sv);
#endif

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
		auto ft = LoadTexture(rootPath + tp.metadata.textureFileName);

		FrameBuffer fb{ true };
		for (auto& fi : tp.frames) {
			//if (!(fi.name == "CossbowHunter_hawk_0008.png"sv || fi.name == "CossbowHunter_hawk_0009.png"sv)) continue;

			auto f = xx::MakeRef<Frame>();
			f->tex = ft;
			f->anchor = fi.anchor.has_value() ? XY{ fi.anchor->x, fi.anchor->y } : XY{ 0.5f, 0.5f };
			f->spriteOffset = { (float)fi.spriteOffset.x, (float)fi.spriteOffset.y };
			f->spriteSize = { (float)fi.spriteSize.width, (float)fi.spriteSize.height };
			f->spriteSourceSize = { (float)fi.spriteSourceSize.width, (float)fi.spriteSourceSize.height };
			if (fi.textureRotated) {
				f->textureRect = { fi.textureRect.x, fi.textureRect.y, fi.textureRect.height, fi.textureRect.width };
				f->textureRotated = true;
			} else {
				f->textureRect = { fi.textureRect.x, fi.textureRect.y, fi.textureRect.width, fi.textureRect.height };
			}

			auto fn = rootPath + fi.name;
			if (fn.find_last_of(".png") == std::string::npos) fn += ".png";

			auto texSiz = f->spriteSourceSize.As<uint32_t>();
			auto t = fb.Draw(texSiz, true, {}, [&] {
				Quad().SetFrame(f).SetScale({ 1, -1 }).SetRotate(f->textureRotated ? gPI / 2 : 0.f)
#if 1
					.SetAnchor({ 0.5f, 0.5f })
					.SetPosition({ f->spriteOffset.MakeFlipY() })
#else
					.SetAnchor(XY{ 0.5f, 0.5f } - f->spriteOffset / XY{ (float)fi.textureRect.width, (float)fi.textureRect.height })		// todo: fix
#endif
					.Draw();
				shader->End();
				//glFlush();
				GLsizei nrChannels = 4;
				GLsizei stride = nrChannels * texSiz.x;
				stride += (stride % 4) ? (4 - stride % 4) : 0;
				GLsizei bufferSize = stride * texSiz.y;
				std::vector<char> buffer(bufferSize);
				glPixelStorei(GL_PACK_ALIGNMENT, 4);
				//glReadBuffer(GL_FRONT);
				glReadPixels(0, 0, texSiz.x, texSiz.y, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
				stbi_flip_vertically_on_write(true);
				stbi_write_png(fn.c_str(), texSiz.x, texSiz.y, nrChannels, buffer.data(), stride);
			});

			if (!fi.aliases.empty()) {
				xx::Data d;
				if (int r = xx::ReadAllBytes(fn, d)) {
					std::cerr << "ReadAllBytes failed. r = " << r << " fn = " << fn << std::endl;
					running = false;
					co_return;
				}
				for (auto& a : fi.aliases) {
					auto afn = rootPath + a;
					if (int r = xx::WriteAllBytes(afn, d)) {
						std::cerr << "WriteAllBytes failed. r = " << r << " fn = " << afn << std::endl;
						running = false;
						co_return;
					}
				}
			}

			Quad().SetFrame(f).Draw();
			co_yield 0;
		}
		running = false;
	}
};

int main(int argc, char** argv) {
	// todo: handle args & store to g
	{
		auto g = new GameLooper();
		auto sg = xx::MakeScopeGuard([&] { delete g; });
		GameLooper::instance = g;
		g->showFps = true;
		//g->showWindow = false;	// todo
		g->title = "xx2dw_tool_plist2png";
		g->Init();
		g->Run<false>();		// break when running == false
	}
	// todo: print info
	xx::CoutN("done. press any to continue...");
	//std::cin.get();

	return 0;
}
