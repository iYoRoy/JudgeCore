#include "SQL.h"

SQL::~SQL()
{
	mysql_close(&sql);
}

bool SQL::Connect(ConnectInfo info)
{
	return mysql_real_connect(&sql, info.host.c_str(), info.user.c_str(), info.passwd.c_str(), info.dbname.c_str(), info.port, NULL, 0);
}

bool SQL::Init()
{
	return mysql_init(&sql);
}

std::string SQL::GetLastError()
{
	return std::string(mysql_error(&sql));
}
