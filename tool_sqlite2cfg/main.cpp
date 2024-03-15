#include <pch.h>

struct EnumData {
	int32_t id{};
	std::string name, desc;
};

inline static bool IsEnumTable(xx::SQLite::TableInfo const& t) {
	return t.columns.size() == 3
		&& t.columns[0].name == "id"sv
		&& t.columns[1].name == "name"sv
		&& t.columns[2].name == "desc"sv;
}

inline static std::vector<EnumData> ReadEnumTableRows(xx::SQLite::Query& q, std::string_view tn) {
	std::vector<EnumData> eds;
	q.SetQuery("select id, name, desc from " + std::string(tn));
	q.Execute([&](xx::SQLite::Reader& r)->int {
		auto& ed = eds.emplace_back();
		ed.id = r.ReadInt32(0);
		ed.name = r.ReadString(1);
		ed.desc = r.ReadString(2);
		return 0;
		});
	return eds;
}

int main() {
	SetConsoleOutputCP(CP_UTF8);

	xx::SQLite::Init();
	xx::SQLite::Connection conn;
	conn.Open("map_td3/cfg.sqlite3");
	if (!conn) {
		xx::CoutN("error code = ", conn.lastErrorCode);
		return __LINE__;
	}
	try {
		// get table names
		auto tns = conn.GetTableNames();

		// get tables info
		std::vector<xx::SQLite::TableInfo> tis;
		for (auto& tn : tns) {
			tis.emplace_back(conn.GetTableInfo(tn));
		}

		// try generate enum code
		std::string s;

		xx::SQLite::Query q(conn);
		for (auto& t : tis) {
			if (!IsEnumTable(t)) continue;

			// fill table data
			auto rows = ReadEnumTableRows(q, t.name);
			xx::Append(s, "enum class ", xx::ToHump(t.name), " : int32_t {\n");
			for (auto& row : rows) {
				xx::AppendFormat(s, "    {1} = {0}, // {2}\n", row.id, row.name, row.desc);
			}
			xx::Append(s, "}\n");
		}

		xx::CoutN(s);

	} catch (std::exception const& ex) {
		xx::CoutN("ex = ", ex.what());
	}
	return 0;
}
