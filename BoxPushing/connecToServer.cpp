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
	return sockfd;		//����һ���Ѿ���������socket
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
	addr.sin_port = htons(RECV_DATABASE_PORT);		//�������ݿ���Ӧ�Ķ˿�,�������˿�
	addr.sin_addr.s_addr = inet_addr(RECV_DATABASE_IP);

	if ((bind(sockfd, (sockaddr*)&addr, sizeof(addr))) == SOCKET_ERROR)
	{
		perrorText("bind");
		return false;
	}

	cout << "�ȴ����ݷ���������......" << endl;
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
	cout << "���ݷ��������ӳɹ�......" << endl;

	return dataBase_sock_tmp;		//����һ���ȴ������ͻ���
}

bool login(userinfo& user) 
{
	int times = 0;
	bool ret = false;

	do
	{
		cout << "�û���:";
		cin >> user.username;

		cout << "����:";
		cin >> user.password;

		
		//����bool��trueΪ��֤�ɹ���falseΪ��֤ʧ��
		ret = fetch_user_info(user);

		if (times > MAX_RETRY_TIMES) {
			cout << "�������������࣬�����˳�" << endl;
			break;
		}
		if (ret == false) {
			times++;
			cout << "�û������������,����������" << endl;
			printf("ʣ��%d�λ���\n", ((MAX_RETRY_TIMES) - times) + 2);
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
	
	cout << "�����˻���֤�������ɹ�......" << endl;
	::system("cls");

	char buf[BUF_LEN] = { 0 };
	snprintf(buf, BUF_LEN, "%s,%s,%s", "l", user.username.c_str(), user.password.c_str());

	if (send(user.Verify_sock, buf, BUF_LEN, 0) == SOCKET_ERROR)
	{	
		closesocket(user.Verify_sock);
		WSACleanup();
		puts("���ʹ���\n");
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
	* �Է������Ļظ����д�������ֻʵ�ֶԵ�¼����Ĵ���
	*/
	if (strcmp(buf, DB_RESP_CMD_LOGIN) != 0)		//�ĳ�switch���ʵ�ֲ�ͬ�ظ���Ĳ�ͬ������ע�ᣬע����
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
	
	if ((user.Verify_sock = init_socket()) < 0)		//�󶨵�����֤�������Ķ˿�
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
	
	//����һ�������׽������ڼ������ݿ����������������
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

	if (debug) printf("��ͼ������׼����������ͼ���� %s\n", level.name.c_str());
	return true;
}
	

bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN])
{
	if (level.map_row > LINE || level.map_col > COLUMN)
	{
		printf("��ͼ���ݴ��󣬵�ͼ����������������Χ\n");
		return false;
	}

	if (level.map_data.length() < 1)
	{
		printf("��ͼ���ݴ�������������\n");
		return false;
	}

	int start = 0, end = 0;
	int row = 0, column = 0;

	do {
		/*
		.find()��һ���ַ������͵ĳ�Ա�������������ַ����в���ָ�����Ӵ�
		.find()��������һ����������Ҫ���ҵ��Ӵ�������ҵ��˸��Ӵ����򷵻ظ��Ӵ���ԭ�ַ����е���ʼλ�ã����򷵻�string::npos����ʾδ�ҵ�
		*/
		end = level.map_data.find("|", start);

		if (end < 0)
		{
			end = level.map_data.length();
			break;
		}

		if (start == end) break;

		/*
		��һ���ַ������͵ĳ�Ա���������ڻ�ȡ�ַ������Ӵ�
		.substr()��������������������һ���������Ӵ�����ʼλ�ã��ڶ����������Ӵ��ĳ���
		���ֻ����һ�����������ʾ��ȡ����ʼλ�õ��ַ���ĩβ���Ӵ�
		��Ҫ����ͷ�ļ�<string.h>
		*/
		string line = level.map_data.substr(start, end - start);

		/*
		����ÿһ��
		strtok_s��һ���ַ��������������ڽ�һ���ַ����ָ�ɶ���Ӵ�
		str��Ҫ�ָ���ַ�����delim�Ƿָ�����context��һ��ָ��ָ���ָ�룬���ڱ���ָ��״̬
		��Ҫ����ͷ�ļ�<string.h>
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
			printf("��ͼ���ݴ��󣬵�ͼ��������\n");
			return false;
		}

		row++;

		if (row > level.map_row) break;

		start = end + 1;
	} while (true);

	if (row < level.map_row)
	{
		printf("��ͼ���������趨�� %d��need��%d��\n", row, level.map_row);
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
			return -1;		//���ݲ����ϱ����趨���ݸ�ʽ
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
			return -1;		//���ݲ����ϱ����趨���ݸ�ʽ
		}

		i++;
	}
	return 0;
}