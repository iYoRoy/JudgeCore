#include "JudgeThread.h"

sem_t JudgeThread::sem;
JudgeInfo JudgeThread::mInfo;
int JudgeThread::nId;

JudgeThread::JudgeThread()
{
	started = false;
}

bool JudgeThread::Create(int id)
{
	if (started)return false;
	started = true;
	nId = id;
	pthread_create(&pth, NULL, ThreadMain, NULL);
	sem_init(&sem, 0, 0);
	return true;
}

bool JudgeThread::PostMessage(std::string ProblemID, std::string SubmitID)
{
	mInfo.ProblemID = ProblemID;
	mInfo.SubmitID = SubmitID;
	sem_post(&sem);
	return false;
}

void* JudgeThread::ThreadMain(void*)
{
	Log log;
	char logMsg[128];
	sprintf(logMsg, "Worker%d", nId);
	log.Print("Worker Started.", logMsg, "I");
	while (sem_wait(&sem)) {
		
	}
	return nullptr;
}
