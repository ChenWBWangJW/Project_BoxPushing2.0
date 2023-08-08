#include "testUtil.h"
#include "sockConn.h"
#include "userDB.h"

#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <thread>
#include <string>

#define DB_CMD_LOGIN 'l'
#define DB_CMD_REGISTER 'r'
#define DB_CMD_DELETE 'd'
#define DB_CMD_GETINFO 'g'
#define DB_CMD_SEND_MAP_DATA 'm'
#define DB_CMD_UDATE_USERINFO 'u'

#define DB_RESP_CMD_HASLOGIN "l,haslogin"
#define DB_RESP_CMD_LOGIN "l,ok"
#define DB_RESP_CMD_REGISTER "r,ok"
#define DB_RESP_CMD_NOEXIST "r,noexist"
#define DB_RESP_CMD_HASREGISTER "r,hasregister"
#define DB_RESP_CMD_DELETE "d,ok"

using namespace std;

void CMDhandel(cmdInfo& CMD)
{
	char respBuf[SIZE_OF_BUF] = "";
	const char* code = CMD.cmd.c_str();
	cout << "recived cmd: " << *code << endl;

	switch (*code)
	{
	case DB_CMD_LOGIN:
		if (login_db(CMD))
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_LOGIN);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Login success!" << endl;
			close(CMD.clientSock);
		}
		else
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_NOEXIST);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Acount noexist" << endl;
			close(CMD.clientSock);
		}
		break;
	case DB_CMD_REGISTER:
		if (register_db(CMD))
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_REGISTER);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Register success!" << endl;
			close(CMD.clientSock);
		}
		else
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_HASREGISTER);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Acount has register" << endl;
			close(CMD.clientSock);
		}
		break;
	case DB_CMD_DELETE:
		if (delele_acount_db(CMD))
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_DELETE);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Delete success!" << endl;
			close(CMD.clientSock);
		}
		else
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_NOEXIST);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Acount noexist" << endl;
			close(CMD.clientSock);
		}
		break;
	case DB_CMD_GETINFO:
		if (get_user_info_db(CMD))
		{
			sprintf(respBuf, "%s,%s", CMD.id.c_str(), CMD.level_id.c_str());
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Userinformation send success!" << endl;
			close(CMD.clientSock);
		}
		else
		{
			sprintf(respBuf, "%s", DB_RESP_CMD_NOEXIST);
			send(CMD.clientSock, respBuf, strlen(respBuf), 0);
			cout << CMD.username << " Acount noexist" << endl;
			close(CMD.clientSock);
		}
		break;
	case DB_CMD_SEND_MAP_DATA:
		if((get_user_info_db(CMD)))
		{
			sendClientInfo(CMD);
			puts("sendClientInfo success.");
			close(CMD.databaseSock);
		}
		else
		{
			puts("sendClientInfo error.");
			close(CMD.databaseSock);
		}
		break;
	case DB_CMD_UDATE_USERINFO:
		if((update_user_level(CMD)))
		{
			puts("upDataUserInfo success.");
			close(CMD.databaseSock);
		}
		else
		{
			puts("upDataUserInfo error.");
			close(CMD.databaseSock);
		}
		break;
	default:
		sprintf(respBuf, "please check you configbook");
		send(CMD.clientSock, respBuf, strlen(respBuf), 0);
		cout << CMD.username << "recived unknown cmd: " << *code << endl;
		close(CMD.clientSock);
	}
	puts("\n");
}

int main()
{
	//1. 创建套接字，返回一个配置完毕的套接字文件描述符
	int sock = -1;
	sock = createSock();

	while (1)
	{
		//2. 通过该套接字从中心服务器获取相关指令和用户数据
		int clientsock = -1;
		cmdInfo CMD;
		if ((clientsock = getClientInfo(sock, CMD)) == -1)
		{
			puts("GetClientInfo error.");
			continue;
		}
		else
		{
			puts("GetClientInfo success.");
		}

		if ((getCMDInfo(clientsock, CMD)) == -1)
		{
			puts("getCMDInfo error.");
			continue;
		}
		else
		{
			puts("getCMDinfo success.");
		}

		//3. 通过相应指令对用户数据进行注册，验证，登录，注销等操作
		std::thread t(CMDhandel, std::ref(CMD));
		t.join();
	}
	
	return 0;
}