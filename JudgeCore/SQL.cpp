#include "SQL.h"

SQL::~SQL()
{
	if (sql != NULL)
		mysql_close(sql);
}

bool SQL::Connect(std::string host, std::string user, std::string passwd, std::string dbname)
{
	mysql_real_connect(sql, host.c_str(), user.c_str(), passwd.c_str(), dbname.c_str(), 0, NULL, 0);
	return !(sql == NULL);
}

bool SQL::Init()
{
	sql = mysql_init(NULL);
	if (sql == NULL) {
		return false;
	}
	return true;
}

std::string SQL::GetLastError()
{
	return std::string(mysql_error(sql));
}
