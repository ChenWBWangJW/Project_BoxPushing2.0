#pragma once

class Task
{
public:
	struct event_base* base = 0;	//libevent���¼������ṹ��

	int sock = -1;

	int thread_id = -1;

	virtual bool Init() = 0;		//���麯��
};
