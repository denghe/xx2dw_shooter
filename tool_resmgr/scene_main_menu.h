#pragma once
#include "pch.h"
#include "db.h"
#include "window_log.h"

namespace Scene_MainMenu {

	enum class Windows : int {
		None = 0,
		File_NewOrOpen,
		File_ImportFiles,
		File_Browse,
	};

	struct Scene : ::ImGuiScene {
		void ImGuiUpdate() override;

		DB db;
		Window_Log wndLog;
		std::map<std::u8string, std::filesystem::path> pngs;

		Windows currentWindow{}, lastWindow{};

		void Draw_File_NewOrOpen();
		std::string dbFileFullPath;

		void Draw_File_ImportFiles();
		xx::Task<> ImportFilesTask;
		xx::Task<> ImportFilesTask_();

		void Draw_File_Browse();
		std::u8string selectedFileName;

		void Draw_Error();
		std::string errorMessage;
	};

}
