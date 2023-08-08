#include "DatabaseTask.h"
#include "testUtil.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include <string.h>
#include <iostream>
using namespace std;

void DatabaseTask::ConnecToSock()
{
	perrorText("At DatabaseTask::ConnecToSock");

	//创建套接字
	if (task->ip.empty() || task->port <= 0 || !task->base)
	{
		cout << "connectoPORT failed!" << endl;
		return;
	}
	if (bev)
	{
		bufferevent_free(bev);
		bev = 0;
	}
	bev = bufferevent_socket_new(task->base, -1, BEV_OPT_CLOSE_ON_FREE);
	sockaddr_in sin;														// 客户端地址
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(task->port);
	evutil_inet_pton(AF_INET, task->ip.c_str(), &sin.sin_addr.s_addr);		// 将字符串ip转换为网络字节序

	// 回调函数与bufferevent绑定
	Setcb(bev);

	// 添加数据通道的超时事件
	timeval t = { 60, 0 };
	bufferevent_set_timeouts(bev, &t, 0);

	// 主动连接客户端
	bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
}

void DatabaseTask::CloseSock()
{
	if (bev)
	{
		bufferevent_free(bev);
		bev = 0;
	}
}

void DatabaseTask::Setcb(bufferevent* bev)
{
	//设置回调函数
	bufferevent_setcb(bev, ReadCB, WriteCB, EventCB, this);

	//添加监控事件
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	//bufferevent_enable(bev, EV_READ);
}

void DatabaseTask::Send(const string& data)
{
	perrorText("At DatabaseTask::Send");
	Send(data.c_str(), data.size());
}

void DatabaseTask::Send(const char* data, size_t datasize)
{
	perrorText("At DatabaseTask::Send");
	if (datasize == 0) return;
	if (bev)
	{
		bufferevent_write(bev, data, datasize);
	}
}

void DatabaseTask::EventCB(bufferevent* bev, short events, void* arg)
{
	DatabaseTask* task = (DatabaseTask*)arg;
	task->Event(bev, events);
}

void DatabaseTask::ReadCB(bufferevent* bev, void* arg)
{
	DatabaseTask* task = (DatabaseTask*)arg;
	task->Read(bev);
}

void DatabaseTask::WriteCB(bufferevent* bev, void* arg)
{
	DatabaseTask* task = (DatabaseTask*)arg;
	task->Write(bev);
}

DatabaseTask::~DatabaseTask()
{
	CloseSock();
}