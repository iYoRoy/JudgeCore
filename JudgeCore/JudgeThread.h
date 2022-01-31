#pragma once
/*
JudgeThreadÀà£ºÅĞ¶¨ºËĞÄ

*/
#include <pthread.h>

class JudgeThread {
public:
	int nId;
	bool Create();
	void Init(int id);
private:
	pthread_t pth;
	bool started;
	int fd;
};