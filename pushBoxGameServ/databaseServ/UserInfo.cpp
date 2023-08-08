#include "UserInfo.h"
#include "testUtil.h"

//libevent相关头文件
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <string>

//数据库相关头文件
#include <cstring>
#include <mysql.h>
#include <time.h>

#include <iostream>
#include <vector>

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "chenweibin980928."
#define DB_NAME "gamedb"

#define SIZE_OF_SQL 256

using namespace std;

void UserInfo::Parse(string msg)
{
	perrorText("UserInfo::Parse");

	vector<string>vals;
	string tmp = "";
	cout << "msg.size(): " << msg.size() << endl;
	for (int i = 0; i < msg.size(); i++)
	{
		if (msg[i] == '.' || msg[i] == ',')		//数字则不进入循环，将该数字字符串写入tmp中
		{
			vals.push_back(tmp);
			tmp = "";
			continue;
		}
		tmp += msg[i];								//将数字写入tmp中
	}

	if (vals.size() != 6)							//循环结束后数组vals中应该有ip地址和端口号的6个数字，若不是则报错
	{
		perrorText("Syntax error in parameters or arguments.\r\n");
		return;
	}

	//解析处ip地址和端口号，并设置在主要流程cmdTask中
	ip = vals[0] + "." + vals[1] + "." + vals[2] + "." + vals[3];		//拼接ip地址
	port = atoi(vals[4].c_str());			//拼接端口号
	level_id = atoi(vals[5].c_str());		//拼接关卡号
	task->ip = ip;													//设置在主要流程task中
	task->port = port;												//设置在主要流程task中
	task->level_id = level_id;										//设置在主要流程task中
	perrorText("IP: " << ip);
	perrorText("PORT: " << port);
	perrorText("LEVEL_ID: " << level_id);

	ConnecToSock();
	ConnecToDB();
	//GetGameData();
	Send(buf.c_str(), buf.size());
	/*
	if (bufferevent_write(bev, buf.c_str(), buf.size()) < 0)
	{
		perrorText("UserInfo::Parse error!");
		return;
	}
	if (buf.empty())
	{
		perrorText("UserInfo::Parse error!");
		return;
	}
	else
	{
		Send(buf.c_str(), buf.size());
		buf.clear();
		CloseSock();
	}
	*/
	//bufferevent_trigger(bev, EV_WRITE, 0);
}

void UserInfo::Read(bufferevent* bev)
{
	perrorText("At UserInfo::Read");

	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0) break;
		cout << "Recv msg(" << len << "): " << data << endl;
		data[len] = '\0';
		Parse(data);
	}
}

void UserInfo::Write(bufferevent* bev)
{
	perrorText("At UserInfo::Write");

	
	if (buf.empty())
	{
		cout << "UserInfo::Write error!" << endl;
		return;
	}

	/*
	* 未解决问题：数据无法成功写出，即客户端无法接收数据
	if ((bufferevent_write(bev, buf.c_str(), buf.size())) < 0)
	{
		perrorText("UserInfo::Write error!");
		return;
	}
	*/

	//Send(buf.c_str(), buf.size());
	buf.clear();
	CloseSock();

	cout << "Map data sent successfully!" << endl;
	cout << flush << endl;
}

bool UserInfo::ConnecToDB()
{
	MYSQL mysql;
	MYSQL_RES* mysql_res;
	MYSQL_ROW mysql_row;
	char sql[SIZE_OF_SQL] = "";
	bool ret = false;

	char map_data[1024] = { 0 };
	string _level_id;
	string _name;
	string _map_row;
	string _map_column;
	string _map_data;
	string _next_level_id;

	if ((mysql_init(&mysql)) == NULL)
	{
		perrorText("MYsql init error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		return false;
	}

	//设置字符编码
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	if ((mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) == NULL)
	{
		perrorText("Mysql connect error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		return false;
	}

	snprintf(sql, SIZE_OF_SQL, "select name, map_row, map_column, map_data, next_level_id from levels where id = '%d';", level_id);
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		printf("return ret :%d, Reason: %s\n", ret, mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}

	mysql_res = mysql_store_result(&mysql);
	if (mysql_res == NULL)
	{
		perrorText("MYSQL store result error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}

	mysql_row = mysql_fetch_row(mysql_res);
	if (mysql_row == NULL)
	{
		perrorText("MYSQL fetch row error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}

	_level_id = to_string(level_id);
	_name = mysql_row[0];
	_map_row = mysql_row[1];
	_map_column = mysql_row[2];
	_map_data = mysql_row[3];
	_next_level_id = mysql_row[4];

	snprintf(map_data, sizeof(map_data), "%s.%s.%s.%s.%s.%s.", _level_id.c_str(), _name.c_str(), _map_row.c_str(), _map_column.c_str(), _map_data.c_str(), _next_level_id.c_str());

	buf = map_data;

	mysql_close(&mysql);
	return true;
}

void UserInfo::GetGameData()
{
	perrorText("At GetGameInfo");

	buf = "1.小试牛刀.9.12.0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,0,0|0,1,0,1,0,1,0,0,1,1,1,0|0,1,0,2,0,1,1,4,1,1,1,0|0,1,1,1,0,3,1,1,1,4,1,0|0,1,2,1,1,4,1,1,1,1,1,0|0,1,0,0,1,0,1,1,0,0,1,0|0,0,0,0,0,0,0,0,0,0,0,0|.2.";
}

void UserInfo::Event(bufferevent* bev, short events)
{
	perrorText("At UserInfo::Event");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		CloseSock();
		delete this;
	}if (events & BEV_EVENT_CONNECTED)
	{
		perrorText("UserInfo::Event BEV_EVENT_CONNECTED");
	}
}

bool UserInfo::Init()
{
	perrorText("At UserInfo::Init");

	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		delete this;
		return false;
	}

	//添加超时
	timeval t = { 300, 0 };
	bufferevent_set_timeouts(bev, &t, 0);

	this->task = this;
	this->bev = bev;

	//注册本对象实现的回调函数
	Setcb(bev);

	return true;
}

UserInfo::UserInfo() {}

UserInfo::~UserInfo() 
{
	CloseSock();
}