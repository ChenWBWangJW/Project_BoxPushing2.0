#include "testUtil.h"
#include "sockConn.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int debug = 1;

int createSock()
{
	int sock;
	int val = 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if ((sock == -1))
	{
		perrorText("Create socket.");
	}
	else
	{
		if (debug) printf("Create socket success.\n");
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(VERIFY_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int))) == -1)
	{
		perrorText("Setsockopt error.");
		return -1;
	}
	else
	{
		if (debug) printf("Setsockopt success.\n");
	}

	if ((bind(sock, (struct sockaddr*)&addr, sizeof(addr))) == -1)
	{
		perrorText("Bind error.");
		return -1;
	}
	else
	{
		if (debug) printf("Bind success.\n");
	}

	if ((listen(sock, 5)) == -1)
	{
		perrorText("Listen error.");
		return -1;
	}
	else
	{
		if (debug) printf("Listen success.\n");
	}

	puts("verfiy server is running...\n");

	return sock;
}

int createSockForDatabase()
{
	int sockForDatabase;
	int val = 1;

	sockForDatabase = socket(AF_INET, SOCK_STREAM, 0);
	if ((sockForDatabase == -1))
	{
		perrorText("Create socket.");
		return -1;
	}
	
	if ((setsockopt(sockForDatabase, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int))) == -1)
	{
		perrorText("Setsockopt error.");
		return -1;
	}

	return sockForDatabase;
}

int getClientInfo(int sock, cmdInfo &CMD)
{
	int clientS;
	char ipAddress[INET_ADDRSTRLEN];
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t clientAddrLen = sizeof(clientAddr);
	
	if ((clientS = accept(sock, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1)
	{
		perrorText("Accept error.");
		return -1;
	}
	else
	{
		if(debug) printf("Accept success.\n");
	}


	
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN) != NULL) {
        CMD.clientIp = ipAddress;
    }
	else 
	{
		perrorText("Inet_ntop error.");
		return -1;
	}
	
	CMD.clientSock = clientS;
	return CMD.clientSock;

}

int sendClientInfo(cmdInfo &CMD)
{
	if ((CMD.databaseSock = createSockForDatabase()) == -1)
	{
		perrorText("Create socket for database error.");
		return -1;
	}
	
	int sendLen = -1;
	char sendBuf[SIZE_OF_BUF];

	struct sockaddr_in databaseServerAddr;
	memset(&databaseServerAddr, 0, sizeof(databaseServerAddr));
	databaseServerAddr.sin_family = AF_INET;
	databaseServerAddr.sin_port = htons(DATABASE_SERV_PORT);
	databaseServerAddr.sin_addr.s_addr = inet_addr(DATABASE_SERV_IP);
	if((connect(CMD.databaseSock, (struct sockaddr*)&databaseServerAddr, sizeof(databaseServerAddr))) == -1)
	{
		perrorText("Connect error.");
		return -1;
	}
	else
	{
		if (debug) printf("Connect Map database success.\n");
	}

	sprintf(sendBuf, "%s,%s,%s.", CMD.clientIp.c_str(), CMD.port.c_str(), CMD.level_id.c_str());
	std::cout << sendBuf << std::endl;
	if ((sendLen = send(CMD.databaseSock, sendBuf, strlen(sendBuf), 0) < 0))
	{
		perrorText("Send error.");
		return -1;
	}
	else

	return 0;
}

int getCMDInfo(int sock, cmdInfo &CMD)
{
	char buf[SIZE_OF_BUF];
	std::string cmd = "null";
	std::string id = "null";
	std::string userName = "null";
	std::string userPasswd = "null";
	std::string client_ip = "null";
	std::string client_port = "null";
	std::string next_level_id = "null";
	

	memset(buf, 0, sizeof(buf));
	int recvLen = 0;
	recvLen = recv(sock, buf, sizeof(buf), 0);
	//传来数据形式应该是：用户名，密码（以逗号分隔）
	if (recvLen == -1)
	{
		perrorText("Recv error.");
	}
	else if (recvLen == 0)
	{
		close(sock);
		if (debug) printf("Client close the connect.");
	}
	{
		if (debug) printf("Recv success.\n");
	}

	std::cout << "buf: " << buf << std::endl;
	if((countChar(buf, ',') != 2)) return -1;		//如果传来的数据不是三位指令
	
	if (buf[0] == 'u')
	{
		if ((saveUserInfo(buf, cmd, next_level_id, id)) == -1)		//将传来的数据分割成指令，用户名和密码
		{
			printf("Save user info error.\n");
			return -1;
		}
		else
		{
			if (debug) printf("Save user info success.\n");
		}

		if (debug) printf("read to copy CMD data to CMDInfo struct.\n");

		if (debug)
		{
			std::cout << "tmpcmd: " << cmd << std::endl;
			std::cout << "tmpnext_level_id: " << next_level_id << std::endl;
			std::cout << "tmpid: " << id << std::endl;
		}

		CMD.cmd = cmd;
		CMD.next_level_id = next_level_id;
		CMD.id = id;

		if (debug)
		{
			std::cout << "CMD.cmd: " << CMD.cmd << std::endl;
			std::cout << "CMD.next_level_id: " << CMD.next_level_id << std::endl;
			std::cout << "CMD.id: " << CMD.id << std::endl;
		}

		return 0;
	}
	else if (buf[0] == 'm')
	{
		if ((saveUserInfo(buf, cmd, userName, client_port)) == -1)		//将传来的数据分割成指令，用户名和密码
		{
			printf("Save user info error.\n");
			return -1;
		}
		else
		{
			if (debug) printf("Save user info success.\n");
		}

		if (debug) printf("read to copy CMD data to CMDInfo struct.\n");

		if (debug)
		{
			std::cout << "tmpcmd: " << cmd << std::endl;
			std::cout << "tmpuserName: " << userName << std::endl;
			std::cout << "tmpclient_port: " << client_port << std::endl;
		}

		CMD.cmd = cmd;
		CMD.username = userName;
		CMD.port = client_port;

		if (debug)
		{
			std::cout << "CMD.cmd: " << CMD.cmd << std::endl;
			std::cout << "CMD.client_ip: " << CMD.clientIp << std::endl;
			std::cout << "CMD.client_port: " << CMD.port << std::endl;
		}

		return 0;
	}

	if ((saveUserInfo(buf, cmd, userName, userPasswd)) == -1)		//将传来的数据分割成指令，用户名和密码
	{
		printf("Save user infomation error.\n");
		return -1;
	}
	else
	{
		if (debug) printf("Save user info success.\n");
	}

	if (debug) printf("read to copy CMD data to CMDInfo struct.\n");

	if (debug)
	{
		std::cout << "tmpcmd: " << cmd << std::endl;
		std::cout << "tmpusername: " << userName << std::endl;
		std::cout << "tmppassword: " << userPasswd << std::endl;
	}

	CMD.cmd = cmd;
	CMD.username = userName;
	CMD.password = userPasswd;

	if (debug)
	{
		std::cout << "CMD.cmd: " << CMD.cmd << std::endl;
		std::cout << "CMD.username: " << CMD.username << std::endl;
		std::cout << "CMD.password: " << CMD.password << std::endl;
	}

	return 0;		//返回每一个请求验证用户信息的客户端的用户信息
}

int saveUserInfo(char buf[], std::string& cmd, std::string& userName, std::string& userPasswd)
{
	std::string c;
	std::string Name;
	std::string Passwd;

	const char * split = ",";
	char *p;
	p = strtok(buf, split);
	int i = 0;
	while (p != NULL)
	{
		// if (debug) printf("%s\n", p);
		if (i == 0) {
			c = p;
			cmd = c;
			printf("in func saveUserInfo: %s\n", p);
			p = strtok(NULL, split);
		}
		else if (i == 1) {
			//strcpy(userName, p);
			Name = p;
			userName = Name;
			std::cout << "in func saveUserInfo: " << userName << std::endl;
			p = strtok(NULL, split);
		}
		else if (i == 2)
		{
			//strcpy(userPasswd, p);
			Passwd = p;
			userPasswd = Passwd;
			std:: cout << "in func saveUserInfo: " << userPasswd << std::endl;
			p = strtok(NULL, split);
		}
		else
		{
			return -1;
		}
		i++;
	}
	return 0;
}

int countChar(const char* p, const char chr)
{
	int count = 0;
	int i = 0;
	while (*(p + i))		//指向单个字符的指针，指针指向的内容不为空
	{
		if (p[i] == chr)
		{
			++count;
		}
		++i;
	}

	return count;
}

void CMDresponse(int sock, char* response)
{
	if ((send(sock, response, strlen(response), 0)) == -1)
	{
		perrorText("Send error.");
	}
	else
	{
		if (debug) printf("Send success.\n");
	}
}