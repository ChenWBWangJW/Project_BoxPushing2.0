#pragma one
#ifndef SOCKCONN_CPP
#define SOCKCONN_CPP

#include <cstdlib>
#include <string>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE_OF_BUF 256
#define VERIFY_PORT 8080
#define DATABASE_SERV_IP "192.168.222.3"
#define DATABASE_SERV_PORT 8081

typedef struct _cmdInfo
{
	std::string cmd;
	std::string id;
	std::string username;
	std::string password;
	std::string level_id;
	std::string next_level_id;
	std::string clientIp;
	std::string port;

	int clientSock = 0;
	int databaseSock = 0;
}cmdInfo;

int createSock();
int createSockForDatabase();
int getClientInfo(int sock, cmdInfo& CMD);
int getCMDInfo(int sock, cmdInfo& CMD);
int saveUserInfo(char[], std::string&, std::string&, std::string&);
int countChar(const char*, const char);
int sendClientInfo(cmdInfo&);
void CMDresponse(int, char*);

#endif // !SOCKCONN_CPP
