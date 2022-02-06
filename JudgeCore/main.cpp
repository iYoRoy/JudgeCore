#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

#include <jsoncpp/json/json.h>

#include "JudgeThread.h"
#include "Log.h"
#include "SQL.h"

std::string strCfgFile;
std::string strDataFolder;
pid_t process_id;
pthread_t pLog, pSql;
pthread_t* pWorker;
SQL db;
Log log;

std::queue<JudgeData>	TaskQueue;
std::queue<std::string>	LogQueue;
std::queue<std::string> SQLQuery;

void* OnWriteLog(void*) {
	LogQueue;
}

void* OnQueryDatabase(void*) {

}

void* OnThreadMain(void*) {

}

int main(int argc, char* argv[])
{
	process_id = getpid();
	strCfgFile = "config.json";
	for (int i = 0; i < argc; i++) {
		if (strstr(argv[i], "--Config-File"))
			strCfgFile = argv[++i];
	}
	Json::Value cfgroot;
	Json::Reader reader;
	std::ifstream ifs_cfg(strCfgFile);
	log.Print("Loading Config...", process_id, "I");
	if (!reader.parse(ifs_cfg, cfgroot)) {
		log.Print("Failed to load Config file!", process_id, "E");
		log.Print("Stop.", process_id, "E");
		ifs_cfg.close();
//		return -1;
	}
	ifs_cfg.close();
	log.Print("Config file Loaded.", process_id, "I");

	log.Print("Connecting to Database...", process_id, "I");
	if (!db.Init()) {
		log.Print("Connection Init FAILED!", process_id, "F");
		log.Print(db.GetLastError(), process_id, "F");
		log.Print("Stop.", process_id, "E");
//		return 0;
	}
	if (!db.Connect({ cfgroot["Database"]["host"].asString(),
					cfgroot["Database"]["user"].asString(),
					cfgroot["Database"]["passwd"].asString(),
					cfgroot["Database"]["dbname"].asString(),
					cfgroot["Database"]["port"].asInt() })) {
		log.Print("Database Connect FAILED!", process_id, "F");
		log.Print(db.GetLastError(), process_id, "F");
		log.Print("Stop.", process_id, "E");
//		return 0;
	}
	log.Print("Database Connected!", process_id, "I");

	log.Print("Starting Threads...", process_id, "I");
	pthread_create(&pth, 0, OnThreadMain, 0);

	return 0;
}
