#include "SQL.h"

SQL::~SQL()
{
	mysql_close(&sql);
}

bool SQL::Connect(std::string host, std::string user, std::string passwd, std::string dbname, int port)
{
	return mysql_real_connect(&sql, host.c_str(), user.c_str(), passwd.c_str(), dbname.c_str(), port, NULL, 0);
}

bool SQL::Init()
{
	return mysql_init(&sql);
}

std::string SQL::GetLastError()
{
	return std::string(mysql_error(&sql));
}
