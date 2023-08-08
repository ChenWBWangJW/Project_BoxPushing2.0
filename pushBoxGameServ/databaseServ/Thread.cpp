#include <thread>
#include <iostream>
using namespace std;
#include <unistd.h>

#include <event2/event.h>

#include "testUtil.h"
#include "Thread.h"
#include "Task.h"

static int debug = 1;

static void Notify_cb(evutil_socket_t fd, short which, void* arg)
{
	Thread* thread = (Thread*)arg;
	thread->Notify(fd, which);
}

void Thread::Notify(evutil_socket_t fd, short which)
{	
	perrorText("Thread ID: " << id << " At Notify");

	char buf[2] = { 0 };			//�ܵ���ȡ������
	int ret = read(fd, buf, 1);		//��ȡ�ܵ�
	if (ret <= 0)
	{
		perrorText("Thread ID: " << id << " At Notify() read pipe error.");
		return;
	}
	if(debug) cout << "Thread ID: " << id << " At Notify() read pipe success.\n" << "Receved: " << buf << endl;
}

void Thread::Start()
{	
	perrorText("Thread ID: " << id << "thread At Start");

	if (Setup())
	{
		if (debug) cout << "Thread ID: " << id << " At Start() Setup() success.\n";
	}

	thread th(&Thread::Main, this);		//�����̣߳���������Ϊ�ö�����
	th.detach();						//�����߳�
}

bool Thread::Setup()
{
	perrorText("Thread ID: " << id << "thread At Setup");

	int fds[2];

	if (pipe(fds))
	{
		if (debug) cerr << "Thread ID: " << id << " At Setup() pipe error.\n";
		return false;
	}

	notify_send_fd = fds[1];		//д��
	event_config * ev_config = event_config_new();
	event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);
	this->base = event_base_new_with_config(ev_config);
	event_config_free(ev_config);

	if (!base)
	{
		if (debug) cerr << "Thread ID: " << id << " At Setup() event_base_new_with_config error.\n";
		return false;
	}

	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, Notify_cb, this);		//�����¼�
	event_add(ev, 0);		//����¼�

	return true;
}

void Thread::Main()
{
	perrorText("Thread ID: " << id << "thread At Main");

	cout << "Thread ID: " << id << " At Main begin" << endl;

	event_base_dispatch(base);		//�¼�ѭ��
	if (debug) cout << "Thread ID: " << id << " At Main() event_base_dispatch() success.\n";
	event_base_free(base);				//�ͷ��¼�

	cout << "Thread ID: " << id << " At Main end" << endl;
}

void Thread::Activate()
{
	perrorText("Thread ID: " << id << " thread At Activate");

	int re = write(notify_send_fd, "c", 1);		//��ܵ�д������
	if (re <= 0)
	{
		cerr << "Thread ID: " << id << " At Activate() write pipe error.\n";
	}

	//�������б���ȡ������
	Task* t = NULL;
	tasks_mutex.lock();
	if (tasks.empty())				//���б�ֱ�ӷ���
	{
		tasks_mutex.unlock();
		return;
	}

	t = tasks.front();				//ȡ������
	tasks.pop_front();				//ɾ���б����ѱ���ȡ��������
	tasks_mutex.unlock();
	t->Init();						//��ʼ������
}

void Thread::AddTask(Task* t)
{
	if(!t) return;

	t->base = this->base;			//�´��������������̳��̳߳����߳��Ѿ��������¼������ṹ��
	tasks_mutex.lock();
	tasks.push_back(t);				//�������
	tasks_mutex.unlock();
}

Thread::Thread()
{
}

Thread::~Thread()
{
}