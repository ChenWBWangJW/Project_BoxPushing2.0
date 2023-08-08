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

//用户信息
typedef struct _userinfo {

	int id;						//用户id
	string username;			//用户名
	string password;			//用户密码
	int level_id;				//用户所在关卡id
	SOCKET Verify_sock = 0;		//验证用socket
	SOCKET Database_sock = 0;	//数据库用socket

}userinfo;

typedef struct _levelinfo {

	int id;				//关卡id
	string name;		//关卡名称
	int map_row;		//地图总行数
	int map_col;		//地图总列数
	string map_data;	//关卡数据
	int next_level_id;	//下一关id

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