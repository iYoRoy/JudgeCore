#include "Log.h"

void Log::Print(std::string str, int workerPID, std::string logtype) {
	char buff[128];
	sprintf(buff, "%s[%s][%d]%s\n", TimeStamp().c_str(), logtype.c_str(), workerPID, str.c_str());
	printf(buff);
	tofile << buff;
	return;
}

Log::Log()
{
	tofile.open("judge.log", std::ios::app);
}

Log::~Log()
{
	tofile.close();
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
