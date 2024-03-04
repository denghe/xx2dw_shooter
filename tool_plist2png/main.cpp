#include <pch.h>

int main(int argc, char** argv) {
	std::filesystem::path fileName, outputDir;
	if (argc == 1) {
		fileName = "C:\\Codes\\KingdomRushFrontiers\\Resources\\towers-hd.plist";
		//outputDir = 
	}
	// todo: parse args to file path
	// todo: read plist content by pugixml

	xx::Data fileData;
	if (int r = xx::ReadAllBytes(fileName, fileData)) return r;

	TexturePackerReader::Plist plist;
	if (int r = plist.Load(fileData)) return r;

	std::cout << "press any key continue...\n";
	std::cin.get();

	return 0;
}
