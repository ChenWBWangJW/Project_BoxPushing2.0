#pragma once

class Task
{
public:
	struct event_base* base = 0;	//libevent的事件基础结构体

	int sock = -1;

	int thread_id = -1;

	virtual bool Init() = 0;		//纯虚函数
};
