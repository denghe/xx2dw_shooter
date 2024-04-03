#include "pch.h"
#include "db.h"

bool DB::OpenOrCreate(std::string path_) {
	conn.Close();
	path.clear();
	conn.Open(path_);
	if (conn) {
		path = std::move(path_);
		CreateTables();
	}
	return conn;
}

bool DB::IsOpened() const {
	return conn;
}

void DB::CreateTables() {
	// todo: if new ? create table
}

