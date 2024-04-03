#pragma once
#include "pch.h"

namespace Scene_MainMenu {

	enum class Windows : int {
		Unknown = 0,
		File_New,
		File_Open,
	};

	struct Scene : ::ImGuiScene {
		void ImGuiUpdate() override;

		std::string dbFileName, errorMessage;

		Windows currentWindow{}, lastWindow{};
		void OpenWindow(Windows w);
		void Draw_File_New();
		void Draw_File_Open();
		void Draw_Error();
	};

}
