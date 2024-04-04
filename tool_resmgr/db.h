#pragma once
#include "pch.h"

struct DB {
	std::filesystem::path rootDir;
	xx::SQLite::Connection conn;
	operator bool() const;
	void NewOrOpen(std::string path);	// conn == true: success
	void Close();
	void CreateTables();
	void CheckFiles();
};
