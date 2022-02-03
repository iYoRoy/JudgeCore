#pragma once
#include <cstring>
#include <string>
#include <mysql/mysql.h>

class SQL {
public:
	~SQL();
	bool Connect(std::string host, std::string user, std::string passwd, std::string dbname, int port);
	bool Init();
	std::string GetLastError();
private:
	MYSQL sql;
	MYSQL_RES* result;
	MYSQL_ROW row;
};