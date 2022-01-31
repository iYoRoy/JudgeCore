#pragma once
#include <string>
#include <fstream>
#include <iostream>

class Log {
public:
	Log();
	~Log();
	void Print(std::string str, std::string logtype);
	void SetOutPut(std::string file);
private:
	std::string writefile;
	std::ofstream ofs;
	bool toFile;
	std::string TimeStamp();
};