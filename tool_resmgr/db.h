#pragma once
#include "pch.h"

struct DB {
	std::string path;
	xx::SQLite::Connection conn;
	bool OpenOrCreate(std::string path_);
	bool IsOpened() const;
	void CreateTables();
};
