#pragma once
/*
JudgeThreadÀà£ºÅÐ¶¨ºËÐÄ

*/
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include "Log.h"

struct JudgeInfo {
	std::string ProblemID;
	std::string SubmitID;
};

class JudgeThread {
public:
	JudgeThread();
	bool Create(int id);
	bool PostMessage(std::string ProblemID,std::string SubmitID);
private:
	static int nId;
	static sem_t sem;
	static JudgeInfo mInfo;
	pthread_t pth;
	bool started;
	static void* ThreadMain(void*);
};