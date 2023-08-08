#pragma once
#ifndef _GAME_DB_H
#define	_GAME_DB_H

#include<string>
#include<winsock.h>
#include<errno.h>
#pragma comment(lib, "ws2_32.lib")

#define VERIFY_PORT 8080
#define VERIFY_IP "192.168.222.3"
#define DATABASE_PORT 8079
#define DATABASE_IP "192.168.222.3"
#define RECV_DATABASE_IP "192.168.222.1"
#define RECV_DATABASE_PORT 8081

#define BUF_LEN 256
#define BUF_OF_MAP_LEN 256
#define MAX_RETRY_TIMES 3
#define LINE 48
#define COLUMN 48


using namespace std;

//�û���Ϣ
typedef struct _userinfo {

	int id;						//�û�id
	string username;			//�û���
	string password;			//�û�����
	int level_id;				//�û����ڹؿ�id
	SOCKET Verify_sock = 0;		//��֤��socket
	SOCKET Database_sock = 0;	//���ݿ���socket

}userinfo;

typedef struct _levelinfo {

	int id;				//�ؿ�id
	string name;		//�ؿ�����
	int map_row;		//��ͼ������
	int map_col;		//��ͼ������
	string map_data;	//�ؿ�����
	int next_level_id;	//��һ��id

}levelinfo;

int init_socket();
int init_socket_for_map();
bool fetch_user_info(userinfo& user);
bool login(userinfo& user);
bool fetch_level_info(userinfo& user, levelinfo& level, int level_id);
bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN]);
bool update_user_level(userinfo& user, int next_level_id);

int get_info(char buf[], int& id, int& level_id);
int get_map_info(char buf[], levelinfo& level);

#endif // !_GAME_DB_H