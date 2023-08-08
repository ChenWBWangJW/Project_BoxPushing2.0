#pragma once
#ifndef _GAME_DB_H
#define	_GAME_DB_H

#include<string>

using namespace std;

#define LINE 48
#define COLUMN 48

//用户信息
typedef struct _userinfo {

	int id;		//用户id
	string username;	//用户名
	string password;	//用户密码
	int level_id;	//用户所在关卡id

}userinfo;

typedef struct _levelinfo {

	int id;		//关卡id
	string name;	//关卡名称
	int map_row;	//地图总行数
	int map_col;	//地图总列数
	string map_data;	//关卡数据
	int next_level_id;	//下一关id

}levelinfo;

bool fetch_user_info(userinfo &user);
bool login(userinfo &user);
bool fetch_level_info(levelinfo &level, int level_id);
bool transfrom_map_db2array(levelinfo &level, int map[LINE][COLUMN]);
bool update_user_level(userinfo &user, int next_level_id);

#endif // !_GAME_DB_H