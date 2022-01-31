#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>

#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

#include "JudgeThread.h"
#include "Log.h"

//全局变量
std::string strCfgFile;
int nThreads;
struct _SQL {
    std::string host,username,passwd,dbname;
}SQL;

int main(int argc, char* argv[])
{
    Log log;
    log.Init();
    strCfgFile = "config.json";
    for (int i = 0; i < argc; i++) {
        if (strstr(argv[i], "--Config-File"))
            strCfgFile = argv[++i];
        if (strstr(argv[i], "--Log-File"))
            log.SetOutPut(argv[++i]);
    }
    log.Print("Loading Config...","Info");
    Json::Value root;
    Json::Reader reader;
    std::ifstream ifs(strCfgFile);
    if (!reader.parse(ifs, root)) {
        log.Print("Failed to load Config file!","Warn");
        log.Print("Use default config file!","Warn");
    }


    return 0;
}