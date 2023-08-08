#pragma once
#include <event2/bufferevent.h>

#include "Task.h"

#include <string>
using namespace std;

struct bufferevent;

class DatabaseTask : public Task
{
public:
	string ip;
	int port;
	int level_id;
	DatabaseTask *task = 0;

	virtual void Parse(string){}

	//建立套接字连接
	void ConnecToSock();

	//关闭连接
	void CloseSock();

	//发送数据
	void Send(const string& data);
	void Send(const char* data, size_t datasize);

	virtual void Event(bufferevent*, short) {}
	virtual void Write(bufferevent*) {}
	virtual void Read(bufferevent*) {}

	void Setcb(struct bufferevent*);
	bool Init() { return true; };

	~DatabaseTask();

protected:
	static void EventCB(bufferevent*, short, void*);
	static void ReadCB(bufferevent*, void*);
	static void WriteCB(bufferevent*, void*);

	bufferevent* bev = 0;
};