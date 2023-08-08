#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;
#include <event2/event.h>
#include <stdlib.h>

#include <signal.h>

#define errmsg(msg) do{cout << msg << endl;exit(1);}while(0)

#include <string>
#include <string.h>
using std::string;
#include <fstream>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <event2/http.h>

#include "ThreadPool.h"
#include "Thread.h"
#include "Task.h"
#include "Factory.h"
#include "testUtil.h"

#define SPORT 8081		//从verify服务器接收的端口号
#define BUFS 1024

#define ThreadPoolGet ThreadPool::Get()

void listen_cb(struct evconnlistener* ev, evutil_socket_t s, struct sockaddr* addr, int socklen, void* arg)
{
	perrorText("main thread listen_cb");

	sockaddr_in* sin = (sockaddr_in*)addr;
	Task* task = Factory::Get()->CreateTask();
	task->sock = s;
	ThreadPoolGet->Dispatch(task);
}

int main()
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		errmsg("signal(SIGPIPE, SIG_IGN) error!");

	ThreadPoolGet->Init(10);

	event_base* base = event_base_new();
	if (!base)
		errmsg("main thread event_base_new failed!");

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	evconnlistener* ev = evconnlistener_new_bind(
		base,											//libevent事件循环对象指针
		listen_cb,
		base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		10,
		(sockaddr*)&sin,
		sizeof(sin));



	if (base)
	{
		cout << "Begin to listen..." << endl;
		event_base_dispatch(base);
	}

	//释放资源
	if (ev)
	{
		evconnlistener_free(ev);
	}
	if (base)
	{
		event_base_free(base);
	}
	perrorText("server end!");
	return 0;
}