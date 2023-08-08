#pragma once

#include <event2/util.h>
#include <list>
#include <mutex>

class Task;
struct event_base;

class Thread
{
public:
	void Start();

	void Main();

	void Notify(evutil_socket_t, short);

	void Activate();

	void AddTask(Task*);

	bool Setup();

	Thread();

	~Thread();

	int id = -1;

private:
	int notify_send_fd = -1;
	event_base *base = 0;
	std::list<Task*> tasks;		//�����б�
	std::mutex tasks_mutex;		//�������������������
};