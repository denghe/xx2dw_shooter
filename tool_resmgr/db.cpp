#include "pch.h"
#include "db.h"

namespace FS = std::filesystem;

DB::operator bool() const {
	return conn;
}

void DB::NewOrOpen(std::string path) {
	assert(!conn);
	std::string_view sv(path);
	if (sv.size() > 1 && sv.starts_with('"') && sv.ends_with('"')) {
		sv = sv.substr(1, sv.size() - 2);
	}
	conn.Open(sv);
	if (conn) {
		FS::path p((std::u8string_view&)sv);
		rootDir = p.remove_filename();
		CreateTables();
		CheckFiles();
	}
}

void DB::Close() {
	conn.Close();
}

void DB::CreateTables() {
	// todo: if new ? create table
	try {

	}
	catch (std::exception const& ex) {
		xx::CoutN("throw exception after conn.Open. ex = ", ex.what());
	}
}

void DB::CheckFiles() {
	// png file name must be unique
}
