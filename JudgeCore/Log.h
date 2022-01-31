#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>

class Log {
public:
	Log();
	~Log();
	void Print(std::string str, std::string worker, std::string logtype);
private:
	pthread_t pth;
	std::string TimeStamp();
};