#pragma once

#include <pthread.h>
#include <cstring>
#include <string>
#include <queue>

struct JudgeData {
	int taskid;
	//	int priority;
	std::string problemID;
	std::string submitID;
	//	bool operator > (const JudgeData& b) const {
	//		if (priority != b.priority)return priority < b.priority;
	//		return taskid < b.taskid;
	//	}
};

class JudgeThread {
public :
	bool Create(int id);
	bool AddTask(JudgeData stData);
	bool Close();
private :
	pthread_t pth;
	static 
};
