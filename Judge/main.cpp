#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <fstream>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <jsoncpp/json/json.h>

std::string strConfig;
	
int main(int argc, char* argv[])
{
	strConfig = "ClientConfig.json";
	Json::Value cfgroot;
	Json::Reader reader;
	std::ifstream ifs_cfg(strConfig);
	if (!reader.parse(ifs_cfg, cfgroot)) {
		std::cout << "Failed to load Config File!" << std::endl;
		return -1;
	}
	ifs_cfg.close();

	int client = socket(AF_INET, SOCK_STREAM, 0);
	if (client == -1) {
		std::cout << "Error: socket" << std::endl;
		return 0;
	}

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(cfgroot["Port"].asInt());
    serverAddr.sin_addr.s_addr = inet_addr(cfgroot["Address"].asString().c_str());
    if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Error: connect" << std::endl;
        return 0;
    }
	char buf[1024];
    while (true) {
		memset(buf, 0, sizeof(buf));
		recv(client, buf, sizeof(buf), 0);
		send(client, argv[1], strlen(argv[1]), 0);
		memset(buf, 0, sizeof(buf));
		recv(client, buf, sizeof(buf), 0);
		send(client, argv[2], strlen(argv[2]), 0);
		memset(buf, 0, sizeof(buf));
		recv(client, buf, sizeof(buf), 0);
		send(client, argv[3], strlen(argv[3]), 0);
		memset(buf, 0, sizeof(buf));
		recv(client, buf, sizeof(buf), 0);
    }
    close(client);

	return 0;
}