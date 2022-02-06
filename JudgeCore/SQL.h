#pragma once
#include <cstring>
#include <string>
#include <mysql/mysql.h>

struct ConnectInfo{
	std::string host;
	std::string user;
	std::string passwd;
	std::string dbname;
	int port;
};

class SQL {
public:
	~SQL();
	bool Connect(ConnectInfo info);
	bool Init();
	std::string GetLastError();
private:
	MYSQL sql;
	MYSQL_RES* result;
	MYSQL_ROW row;
};