#pragma once
#include <cstring>
#include <string>

class SQL {
public:
	SQL();
	~SQL();
	bool Connect(std::string host, std::string user, std::string passwd, std::string dbname);
private:
};