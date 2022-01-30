#include "JudgeThread.h"

bool JudgeThread::Create()
{
	started = true;

	return false;
}

void JudgeThread::Init(int id)
{
	started = false;
	nId = id;
}
