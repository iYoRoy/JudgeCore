#include "Log.h"
void Log::Print(std::string str,std::string logtype) {
	std::cout << TimeStamp() << '[' << logtype << ']' << str << std::endl;
	if (toFile) {
		ofs << TimeStamp() << '[' << logtype << ']' << str << std::endl;
	}
}

void Log::SetOutPut(std::string file)
{
	writefile = file;
	ofs.open(file);
}

Log::Log()
{
	writefile = "";
	toFile = false;
}

Log::~Log()
{
	ofs.close();
	toFile = false;
}

std::string Log::TimeStamp()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	tm nowTime;
	localtime_r(&time.tv_sec, &nowTime);
	char current[32];
	sprintf(current, "[%02d:%02d:%02d]",
		nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
	return std::string(current);
}
