#include <pch.h>

int main() {
	xx::SQLite::Init();
	xx::SQLite::Connection conn;
    conn.Open("map_td3/cfg.sqlite3");
    if (!conn) {
        xx::CoutN("error code = ", conn.lastErrorCode);
        return __LINE__;
    }
    try {
        xx::CoutN("num tables = ", conn.GetTableCount());
        
    } catch (std::exception const& ex) {
        xx::CoutN("ex = ", ex.what());
    }
	return 0;
}
