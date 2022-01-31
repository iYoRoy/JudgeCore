#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>

#include <jsoncpp/json/json.h>

#include "JudgeThread.h"
#include "Log.h"
#include "SQL.h"

std::string strCfgFile;
std::string strDataFolder;
JudgeThread* ths;
int nThreads;
int Timeout_Sec;
SQL db;

int main(int argc, char* argv[])
{
	Log log;
	strCfgFile = "config.json";
	for (int i = 0; i < argc; i++) {
		if (strstr(argv[i], "--Config-File"))
			strCfgFile = argv[++i];
	}
	log.Print("Loading Config...", "Main", "I");
	Json::Value root;
	Json::Reader reader; 
	std::ifstream ifs(strCfgFile);
	if (!reader.parse(ifs, root)) {
		log.Print("Failed to load Config file!", "Main", "E");
		log.Print("Service Stop.", "Main", "E");
//		return -1;
	}
	nThreads = (root["WorkerThreads"].asInt() != 0 ? root["WorkerThreads"].asInt() : sysconf(_SC_NPROCESSORS_ONLN));
	strDataFolder = root["DataFolder"].asString();
	Timeout_Sec = root["TimeOut"].asInt();
	log.Print("Config file Loaded.", "Main", "I");

	log.Print("Connect to Database...", "Main", "I");
	if (!db.Init()) {
		log.Print("Database INIT FAILED!", "Main", "F");
		log.Print(db.GetLastError(), "Main", "F");
		log.Print("Service Stop.", "Main", "E");
//		return 0;
	}
	if (!db.Connect(root["Database"]["host"].asString(), root["Database"]["user"].asString(), root["Database"]["passwd"].asString(), root["Database"]["dbname"].asString())) {
		log.Print("Database Connect FAILED!", "Main", "F");
		log.Print(db.GetLastError(), "Main", "F");
		log.Print("Service Stop.", "Main", "E");
//		return 0;
	}
	log.Print("Database Connected!", "Main", "I");
	log.Print("Starting Worker threads...", "Main", "I");
	ths = new JudgeThread[nThreads];
	for (int i = 0; i < nThreads; i++) {
		char bufferStr[64];
		sprintf(bufferStr, "Starting Worker%d...", i);
		log.Print(bufferStr, "Main", "I");
		ths[i].Create(i);
	}
	


	delete[] ths;
	return 0;
}
