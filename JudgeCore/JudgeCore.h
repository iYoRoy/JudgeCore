#pragma once
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <csignal>
#include <jsoncpp/json/json.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Log.h"
#include "SQL.h"

#define _DEBUG
//#define _SIGNAL_TH

#define DEFAULT_PORT	20495
#define DEFAULT_QUEUE	30

struct JudgeData {
	int id;
	std::string ProblemID;
	std::string SubmitID;
	std::string Language;
};
struct LogInfo {
	std::string content;
	std::string worker;
	std::string type;
};
struct JudgeResult {
	std::string SubmitID;
	std::string Status;
};

std::string strCfgFile;
std::string strDataFolder;
std::string strWelcome;
pthread_t pLog, pSql, * pWorker;
SQL db;
Log log;
bool MainThreadStart, QueryThreadStart;
bool QueryThreadExist, * MainThreadExist;
bool isQueryThreadExit, isThreadExit;
int numThreads;
int ConnectPort;
int ConnectionQueueLength;

std::queue<JudgeData>	TaskQueue;
std::queue<LogInfo>		LogQueue;
std::queue<JudgeResult> SQLQuery;
std::mutex				TaskQueueLock;
std::mutex				LogQueueLock;
std::mutex				SQLQueryLock;

void* OnWriteLog(void*);
void* OnQueryDatabase(void*);
void* OnThreadMain(void* lpVoid);
void DoExitClear();
void signalCallback(int sig);
int main(int argc, char* argv[]);