#include "JudgeCore.h"

void* OnWriteLog(void*) {
	LogInfo tlg;
	while (true) {
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
	while (QueryThreadExist) {
		SQLQueryLock.lock();
		if (SQLQuery.empty()) {
			usleep(10);
			SQLQueryLock.unlock();
			continue;
		}
		SQLQueryLock.unlock();
	}

	isThreadExit = true;

	return 0;
}

void* OnThreadMain(void* lpVoid) {
	char namebuf[8];
	char logbuf[32];
	int workerID = (long)lpVoid;
	JudgeData jdData;
	sprintf(namebuf, "Worker%d", workerID);

	LogQueueLock.lock();
	LogQueue.push({ "Thread Started!", namebuf, "I" });
	LogQueueLock.unlock();
	
	while (MainThreadExist[workerID]) {
		TaskQueueLock.lock();
		if (TaskQueue.empty()) {
			usleep(10);
			TaskQueueLock.unlock();
			continue;
		}
		jdData = TaskQueue.front();
		TaskQueue.pop();
		TaskQueueLock.unlock();

		sprintf(logbuf, "Catch Task:%d", jdData.id);
		LogQueueLock.lock();
		LogQueue.push({ logbuf, namebuf, "I" });
		LogQueueLock.unlock();


	}

	LogQueueLock.lock();
	LogQueue.push({ "Thread Exit.", namebuf, "I" });
	LogQueueLock.unlock();

	isThreadExit = true;

	return 0;
}

void DoExitClear() {
	if (MainThreadStart) {
		for (int i = 0; i < numThreads; i++) {
			isThreadExit = false;
			MainThreadExist[i] = false;
			while (!isThreadExit)usleep(100);
		}
	}
	if (QueryThreadStart) {
		isThreadExit = false;
		QueryThreadExist = false;
		while (!isThreadExit)usleep(100);
	}
	return;
}

void signalCallback(int sig) {
	switch (sig) {
	case 2: {
		LogQueueLock.lock();
		LogQueue.push({ "Catch Exit Signal.", "Main", "I" });
		LogQueue.push({ "Waiting for Thread(s) exit...", "Main", "I" });
		LogQueueLock.unlock();
		DoExitClear();
		exit(0);
	}
	}
}

int main(int argc, char* argv[])
{
	QueryThreadExist = false;
	QueryThreadStart = false;
	MainThreadStart = false;

	signal(SIGINT, signalCallback);
	pthread_create(&pLog, NULL, OnWriteLog, NULL);
	
	strCfgFile = "ServerConfig.json";
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
#ifndef _DEBUG
		return -1;
#endif
	}
	ifs_cfg.close();
	ConnectPort = (cfgroot["Bind"]["Port"].isNull() ? DEFAULT_PORT : cfgroot["Bind"]["Port"].asInt());
	ConnectionQueueLength = (cfgroot["Bind"]["QueueLength"].isNull() ? DEFAULT_QUEUE : cfgroot["Bind"]["QueueLength"].asInt());
	numThreads = (cfgroot["ThreadNum"].isNull() ? sysconf(_SC_NPROCESSORS_ONLN) : cfgroot["ThreadNum"].asInt());
	strWelcome = (cfgroot["Welcome"].isNull() ? "JudgeCore-v0.1.0 Connect ID:%d\n" : cfgroot["Welcome"].asString());

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
#ifndef _DEBUG
		return 0;
#endif
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
#ifndef _DEBUG
		return 0;
#endif
	}

	LogQueueLock.lock();
	LogQueue.push({ "Database Connected!", "Main", "I" });
	LogQueue.push({ "Starting Thread(s)...", "Main", "I" });
	LogQueueLock.unlock();

	pWorker = new pthread_t[numThreads];
	MainThreadExist = new bool[numThreads];
	for (int i = 0; i < numThreads; i++) {
		MainThreadExist[i] = true;
		pthread_create(&pWorker[i], NULL, OnThreadMain, (void*)i);
	}
	MainThreadStart = true;
	QueryThreadExist = true;
	pthread_create(&pSql, NULL, OnQueryDatabase, NULL);
	QueryThreadStart = true;

	LogQueueLock.lock();
	LogQueue.push({ "Thread(s) Started!", "Main", "I" });
	LogQueue.push({ "Listening port...", "Main", "I" });
	LogQueueLock.unlock();

	fd_set rfds;
	timeval tv;
	int retval, maxfd;
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(ConnectPort);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd, (sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1) {
		LogQueueLock.lock();
		LogQueue.push({ "Failed to bind!", "Main", "F" });
		LogQueue.push({ "Stop.", "Main", "F" });
		LogQueueLock.unlock();
		perror("bind");
		DoExitClear();
		exit(1);
	}
	if (listen(listenfd, ConnectionQueueLength) == -1) {
		LogQueueLock.lock();
		LogQueue.push({ "Failed to listen!", "Main", "F" });
		LogQueue.push({ "Stop.", "Main", "F" });
		LogQueueLock.unlock();
		perror("listen");
		DoExitClear();
		exit(1);
	}

	LogQueueLock.lock();
	LogQueue.push({ "Service Started!", "Main", "I" });
	LogQueueLock.unlock();

	sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int conn;
	int id=0;
	while (true) {
		conn = accept(listenfd, (struct sockaddr*)&client_addr, &length);
		if (conn < 0)
		{
			LogQueueLock.lock();
			LogQueue.push({ "Failed to accept connection!", "Main", "W" });
			LogQueueLock.unlock();
			continue;
		}

		char buf[1024];
		int len;
		JudgeData jdDt;
		jdDt.id = id;
		memset(buf, 0, sizeof(buf));
		sprintf(buf, strWelcome.c_str(), id);
		send(conn, buf, strlen(buf), 0);
		memset(buf, 0, sizeof(buf));
		len = recv(conn, buf, sizeof(buf), 0);
		buf[len] = '\0';
		jdDt.Language = buf;
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Language Set:%s", jdDt.Language.c_str());
		send(conn, buf, strlen(buf), 0);
		memset(buf, 0, sizeof(buf));
		len = recv(conn, buf, sizeof(buf), 0);
		buf[len] = '\0';
		jdDt.ProblemID = buf;
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Select ProblemID:%s", jdDt.ProblemID.c_str());
		send(conn, buf, strlen(buf), 0);
		memset(buf, 0, sizeof(buf));
		len = recv(conn, buf, sizeof(buf), 0);
		buf[len] = '\0';
		jdDt.SubmitID = buf;
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Select SubmitID:%s\nDone.", jdDt.SubmitID.c_str());
		send(conn, buf, strlen(buf), 0);

		close(conn);
		TaskQueueLock.lock();
		TaskQueue.push(jdDt);
		TaskQueueLock.unlock();

		char logbuf[32];
		sprintf(logbuf, "Task (id:%d) assigned.", id++);
		LogQueueLock.lock();
		LogQueue.push({ logbuf, "Main", "I" });
		LogQueueLock.unlock();
	}

	return 0;
}
