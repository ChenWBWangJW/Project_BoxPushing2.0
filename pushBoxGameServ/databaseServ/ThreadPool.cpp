#include "ThreadPool.h"
#include "Thread.h"
#include "Task.h"
#include <thread>
#include <iostream>
using namespace std;
#include "testUtil.h"

void ThreadPool::Dispatch(Task* task)
{
	perrorText("At ThreadPool::Dispatch");
	thread_index = (thread_index + 1) % thread_num;		//�߳�������1
	Thread *t = threads[thread_index];					//ȡ���߳�

	t->AddTask(task);										//�������
	t->Activate();										//�����߳�
}

void ThreadPool::Init(int threads_num)
{
	perrorText("At ThreadPool::Init");

	this->thread_num = threads_num;						//�߳�����
	this->thread_index = -1;								//�߳�����

	for (int i = 0; i < threads_num; i++)
	{
		cout << "Create thread " << i+1 << endl;
		Thread* t = new Thread();							//�����߳�
		t->id = i+1;										//�����߳�ID
		t->Start();											//�����߳�
		threads.push_back(t);								//��ӵ��߳��б�
		this_thread::sleep_for(chrono::milliseconds(100));	//����100ms
	}
}