#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <queue>
#include <mutex>

#include <jsoncpp/json/json.h>

#include "Log.h"
#include "SQL.h"

struct JudgeData {
	int id;
	std::string ProblemID;
	std::string SubmitID;
};
struct LogInfo {
	std::string content;
	std::string worker;
	std::string type;
};

std::string strCfgFile;
std::string strDataFolder;
pthread_t pLog, pSql, * pWorker;
SQL db;
Log log;
bool QueryThread, * MainThread, LogThread;
int numThreads;

std::queue<JudgeData>	TaskQueue;
std::queue<LogInfo>		LogQueue;
std::queue<std::string> SQLQuery;
std::mutex				TaskQueueLock;
std::mutex				LogQueueLock;
std::mutex				SQLQueryLock;

void* OnWriteLog(void*) {
	LogInfo tlg;
	while (LogThread) {
		LogQueueLock.lock();
		if (LogQueue.empty()) {
			usleep(10);
			LogQueueLock.unlock();
			continue;
		}
		tlg = LogQueue.front();
		LogQueue.pop();
		LogQueueLock.unlock();
		log.Print(tlg.content, tlg.worker, tlg.type);
	}
	return 0;
}

void* OnQueryDatabase(void*) {
	while (QueryThread) {
		SQLQueryLock.lock();
		if (SQLQuery.empty()) {
			usleep(10);
			SQLQueryLock.unlock();
			continue;
		}
	}
	return 0;
}

void* OnThreadMain(void* lpVoid) {
	char namebuf[32];
	int workerID = (long)lpVoid;
	sprintf(namebuf, "Worker%d", workerID);

	LogQueueLock.lock();
	LogQueue.push({ "Thread Started!", namebuf, "I" });
	LogQueueLock.unlock();
	
	while (MainThread) {
		TaskQueueLock.lock();
		if (TaskQueue.empty()) {
			usleep(10);
			TaskQueueLock.unlock();
			continue;
		}
	}
	
	LogQueueLock.lock();
	LogQueue.push({ "Thread Exit.", namebuf, "I" });
	LogQueueLock.unlock();

	return 0;
}

void signalCallback(int sig) {
	switch (sig) {
	case 2: {
		LogQueueLock.lock();
		LogQueue.push({ "Catch Exit Signal.", "Main", "I" });
		LogQueue.push({ "Service Stop.", "Main", "I" });
		LogQueueLock.unlock();
		for (int i = 0; i < numThreads; i++) 
			MainThread[i] = false; 
		QueryThread = false;
		LogThread = false;
		break;
	}
	}
}

int main(int argc, char* argv[])
{
	LogThread = true;
	pthread_create(&pLog, NULL, OnWriteLog, NULL);
	
	strCfgFile = "config.json";
	for (int i = 0; i < argc; i++) {
		if (strstr(argv[i], "--Config-File"))
			strCfgFile = argv[++i];
	}
	
	Json::Value cfgroot;
	Json::Reader reader;
	std::ifstream ifs_cfg(strCfgFile);

	LogQueueLock.lock();
	LogQueue.push({ "Loading Config...", "Main", "I" });
	LogQueueLock.unlock();
	
	if (!reader.parse(ifs_cfg, cfgroot)) {
		LogQueueLock.lock();
		LogQueue.push({ "Failed to load Config file!", "Main", "E" });
		LogQueue.push({ "Stop.", "Main", "E" });
		LogQueueLock.unlock();

		ifs_cfg.close();
//		return -1;
	}
	ifs_cfg.close();

	numThreads = (cfgroot["ThreadNum"].isNull() ? sysconf(_SC_NPROCESSORS_ONLN) : cfgroot["ThreadNum"].asInt());

	LogQueueLock.lock();
	LogQueue.push({ "Config file Loaded.", "Main", "I" });
	LogQueue.push({ "Connecting to Database...", "Main", "I" });
	LogQueueLock.unlock();

	if (!db.Init()) {
		LogQueueLock.lock();
		LogQueue.push({ "Connection Init FAILED!", "Main", "F" });
		LogQueue.push({ db.GetLastError(), "Main", "F" });
		LogQueue.push({ "Stop.", "Main", "E" });
		LogQueueLock.unlock();
//		return 0;
	}
	if (!db.Connect({ cfgroot["Database"]["host"].asString(),
					cfgroot["Database"]["user"].asString(),
					cfgroot["Database"]["passwd"].asString(),
					cfgroot["Database"]["dbname"].asString(),
					cfgroot["Database"]["port"].asInt() })) {
		LogQueueLock.lock();
		LogQueue.push({ "Database Connect FAILED!", "Main", "F" });
		LogQueue.push({ db.GetLastError(), "Main", "F" });
		LogQueue.push({ "Stop.", "Main", "E" });
		LogQueueLock.unlock();
//		return 0;
	}

	LogQueueLock.lock();
	LogQueue.push({ "Database Connected!", "Main", "I" });
	LogQueue.push({ "Starting Thread(s)...", "Main", "I" });
	LogQueueLock.unlock();

	pWorker = new pthread_t[numThreads];
	MainThread = new bool[numThreads];
	for (int i = 0; i < numThreads; i++) {
		MainThread[i] = true;
		pthread_create(&pWorker[i], NULL, OnThreadMain, (void*)i);
	}

	LogQueueLock.lock();
	LogQueue.push({ "Thread(s) Started!", "Main", "I" });
	LogQueue.push({ "", "Main", "I" });
	LogQueueLock.unlock();

	

	return 0;
}
