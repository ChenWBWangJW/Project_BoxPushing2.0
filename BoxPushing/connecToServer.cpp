#include "connecToServer.h"

#include <iostream>

#define DB_RESP_CMD_HASLOGIN "l,haslogin"
#define DB_RESP_CMD_LOGIN "l,ok"
#define DB_RESP_CMD_REGISTER "r,ok"
#define DB_RESP_CMD_NOEXIST "r,noexist"
#define DB_RESP_CMD_HASREGISTER "r,hasregister"
#define DB_RESP_CMD_DELETE "d,ok"

static int debug = 1;

void perrorText(const char* str)
{
	fprintf(stderr, "%s error. reason code: %d\n", str, WSAGetLastError());
	exit(-1);
}

int init_socket()
{
	WSADATA wasdata;
	if ((WSAStartup(MAKEWORD(2, 2), &wasdata)) != 0)
	{
		perrorText("WSAStartup");
		return -1;
	}
		
	
	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == INVALID_SOCKET)
	{
		perrorText("socket");
		return -1;
	}
	
	int opt = 1;
	if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) == SOCKET_ERROR)
	{
		perrorText("setsockopt");
		return false;
	}

	sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(VERIFY_PORT);
	servaddr.sin_addr.s_addr = inet_addr(VERIFY_IP);

	if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		perrorText("connect");
		return -1;
	}

	//user.Verify_sock = sockfd;
	return sockfd;		//返回一个已经建立连接socket
}

int init_socket_for_map()
{
	WSADATA wasdata;
	if ((WSAStartup(MAKEWORD(2, 2), &wasdata)) != 0)
	{
		perrorText("WSAStartup");
		return -1;
	}


	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == INVALID_SOCKET)
	{
		perrorText("socket");
		return -1;
	}

	int opt = 1;
	if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) == SOCKET_ERROR)
	{
		perrorText("setsockopt");
		return false;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(RECV_DATABASE_PORT);		//接收数据库响应的端口,即本机端口
	addr.sin_addr.s_addr = inet_addr(RECV_DATABASE_IP);

	if ((bind(sockfd, (sockaddr*)&addr, sizeof(addr))) == SOCKET_ERROR)
	{
		perrorText("bind");
		return false;
	}

	cout << "等待数据服务器连接......" << endl;
	if ((listen(sockfd, 5)) == SOCKET_ERROR)
	{
		perrorText("listen");
		return false;
	}

	sockaddr_in database_addr;
	memset(&database_addr, 0, sizeof(database_addr));
	int addrlen = sizeof(database_addr);

	SOCKET dataBase_sock_tmp = -1;
	if ((dataBase_sock_tmp = (accept(sockfd, (sockaddr*)&database_addr, &addrlen))) == SOCKET_ERROR)
	{
		perrorText("accept");
		return false;
	}
	cout << "数据服务器连接成功......" << endl;

	return dataBase_sock_tmp;		//返回一个等待监听客户端
}

bool login(userinfo& user) 
{
	int times = 0;
	bool ret = false;

	do
	{
		cout << "用户名:";
		cin >> user.username;

		cout << "密码:";
		cin >> user.password;

		
		//返回bool，true为验证成功，false为验证失败
		ret = fetch_user_info(user);

		if (times > MAX_RETRY_TIMES) {
			cout << "输入错误次数过多，程序退出" << endl;
			break;
		}
		if (ret == false) {
			times++;
			cout << "用户名或密码错误,请重新输入" << endl;
			printf("剩余%d次机会\n", ((MAX_RETRY_TIMES) - times) + 2);
		}
	} while (!ret);

	return ret;
}


bool acount_verify(userinfo& user)
{
	if ((user.Verify_sock = init_socket()) < 0)
	{
		printf("init_socket failed.");
		return false;
	}
	
	cout << "连接账户验证服务器成功......" << endl;
	::system("cls");

	char buf[BUF_LEN] = { 0 };
	snprintf(buf, BUF_LEN, "%s,%s,%s", "l", user.username.c_str(), user.password.c_str());

	if (send(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{	
		closesocket(user.Verify_sock);
		WSACleanup();
		puts("发送错误\n");
		perrorText("send");
		return false;
	}

	memset(buf, 0, BUF_LEN);

	if (recv(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		perrorText("recv");
		return false;
	}

	/*
	* 对服务器的回复进行处理，暂且只实现对登录请求的处理
	*/
	if (strcmp(buf, DB_RESP_CMD_LOGIN) != 0)		//改成switch后可实现不同回复码的不同处理，如注册，注销等
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		return false;
	}
		
	closesocket(user.Verify_sock);
	WSACleanup();
	return true;
}


bool fetch_user_info(userinfo& user)
{
	if((acount_verify(user)) == false)
		return false;
	
	if ((user.Verify_sock = init_socket()) < 0)		//绑定的是验证服务器的端口
	{
		printf("init_socket failed.");
		return false;
	}

	char buf[BUF_LEN] = { 0 };
	int id = -1;
	int level_id = -1;
	snprintf(buf, BUF_LEN, "%s,%s,%s", "g", user.username.c_str(), user.password.c_str());

	if (send(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		perrorText("send");
		return false;
	}

	memset(buf, 0, BUF_LEN);

	if (recv(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		perrorText("recv");
		return false;
	}

	
	if ((get_info(buf, id, level_id)) == -1)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		return false;
	}

	user.id = id;
	user.level_id = level_id;

	closesocket(user.Verify_sock);
	WSACleanup();
	return true;
}

bool update_user_level(userinfo& user, int next_level_id)
{
	if ((user.Verify_sock = init_socket()) < 0)
	{
		printf("init_socket failed.");
		return false;
	}

	char buf[BUF_LEN] = { 0 };
	memset(buf, 0, BUF_LEN);
	snprintf(buf, BUF_LEN, "%s,%d,%d", "u", next_level_id, user.id);

	if (send(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		perrorText("send");
		return false;
	}

	closesocket(user.Verify_sock);
	WSACleanup();
	return true;
}

bool send_map_quest(userinfo& user)
{
	if ((user.Verify_sock = init_socket()) < 0)
	{
		printf("init_socket failed.");
		return false;
	}

	string recv_database_port = to_string(RECV_DATABASE_PORT);
	char buf[BUF_LEN] = { 0 };
	memset(buf, 0, BUF_LEN);
	snprintf(buf, BUF_LEN, "%s,%s,%s", "m", user.username.c_str(), recv_database_port.c_str());

	if (send(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{
		closesocket(user.Verify_sock);
		WSACleanup();
		perrorText("send");
		return false;
	}

	closesocket(user.Verify_sock);
	WSACleanup();
	return true;
}

bool fetch_level_info(userinfo& user ,levelinfo& level, int level_id) 
{
	if ((send_map_quest(user)) == false)
	{
		printf("send_map_quest failed.");
		return false;
	}
	
	//创建一个监听套接字用于监听数据库服务器的连接请求
	if ((user.Database_sock = init_socket_for_map()) < 0)
	{
		printf("init_socket failed.");
		return false;
	}

	char buf[BUF_OF_MAP_LEN] = { 0 };
	if (recv(user.Database_sock, buf, BUF_OF_MAP_LEN, 0) == SOCKET_ERROR)
	{
		perrorText("recv");
		return false;
	}
	closesocket(user.Database_sock);
	WSACleanup();

	if((get_map_info(buf, level)) == -1)
		return false;

	if (debug) printf("地图数据已准备就绪，地图名： %s\n", level.name.c_str());
	return true;
}
	

bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN])
{
	if (level.map_row > LINE || level.map_col > COLUMN)
	{
		printf("地图数据错误，地图行数或列数超出范围\n");
		return false;
	}

	if (level.map_data.length() < 1)
	{
		printf("地图数据错误，请重新设置\n");
		return false;
	}

	int start = 0, end = 0;
	int row = 0, column = 0;

	do {
		/*
		.find()是一个字符串类型的成员函数，用于在字符串中查找指定的子串
		.find()函数接受一个参数，即要查找的子串。如果找到了该子串，则返回该子串在原字符串中的起始位置；否则返回string::npos，表示未找到
		*/
		end = level.map_data.find("|", start);

		if (end < 0)
		{
			end = level.map_data.length();
			break;
		}

		if (start == end) break;

		/*
		是一个字符串类型的成员函数，用于获取字符串的子串
		.substr()函数接受两个参数，第一个参数是子串的起始位置，第二个参数是子串的长度
		如果只传递一个参数，则表示获取从起始位置到字符串末尾的子串
		需要包含头文件<string.h>
		*/
		string line = level.map_data.substr(start, end - start);

		/*
		解析每一行
		strtok_s是一个字符串处理函数，用于将一个字符串分割成多个子串
		str是要分割的字符串，delim是分隔符，context是一个指向指针的指针，用于保存分割的状态
		需要包含头文件<string.h>
		//0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,0,0........
		*/
		char* next_token = NULL;
		char* item = strtok_s((char*)line.c_str(), ",", &next_token);

		column = 0;

		while (item && column < level.map_col)
		{
			map[row][column] = atoi(item);
			column++;
			item = strtok_s(NULL, ",", &next_token);
		}

		if (column < level.map_col)
		{
			printf("地图数据错误，地图列数不足\n");
			return false;
		}

		row++;

		if (row > level.map_row) break;

		start = end + 1;
	} while (true);

	if (row < level.map_row)
	{
		printf("地图行数少于设定， %d（need：%d）\n", row, level.map_row);
		return false;
	}

	return true;
}

int get_info(char buf[], int& id, int& level_id)
{
	const char* split = ",";
	char * p = NULL;
	char * next_p = NULL;
	p = strtok_s(buf, split, &next_p);
	int i = 0;
	while (p != NULL)
	{
		if (i == 0)
		{
			id = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 1)
		{
			level_id = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else
		{
			return -1;		//传递不符合报文设定数据格式
		}
			
		i++;
	}
	return 0;
}

/*
level.id = level_id;
level.name = mysql_row[0];
level.map_row = atoi(mysql_row[1]);
level.map_col = atoi(mysql_row[2]);
level.map_data = mysql_row[3];
level.next_level_id = atoi(mysql_row[4]);
*/
int get_map_info(char buf[], levelinfo& level)
{
	const char* split = ".";
	char* p = NULL;
	char* next_p = NULL;
	p = strtok_s(buf, split, &next_p);
	int i = 0;
	while (p != NULL)
	{
		if (i == 0)
		{
			level.id = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 1)
		{
			level.name = p;
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 2)
		{
			level.map_row = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 3)
		{
			level.map_col = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 4)
		{
			level.map_data = p;
			p = strtok_s(NULL, split, &next_p);
		}
		else if (i == 5)
		{
			level.next_level_id = atoi(p);
			p = strtok_s(NULL, split, &next_p);
		}
		else
		{
			return -1;		//传递不符合报文设定数据格式
		}

		i++;
	}
	return 0;
}