#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <jsoncpp/json/json.h>

class Log {
public:
	Log();
	~Log();
	void Print(std::string str, int workerPID, std::string logtype);
private:
	std::string TimeStamp();
	std::ofstream tofile;
};