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
	thread_index = (thread_index + 1) % thread_num;		//线程索引加1
	Thread *t = threads[thread_index];					//取出线程

	t->AddTask(task);										//添加任务
	t->Activate();										//激活线程
}

void ThreadPool::Init(int threads_num)
{
	perrorText("At ThreadPool::Init");

	this->thread_num = threads_num;						//线程数量
	this->thread_index = -1;								//线程索引

	for (int i = 0; i < threads_num; i++)
	{
		cout << "Create thread " << i+1 << endl;
		Thread* t = new Thread();							//创建线程
		t->id = i+1;										//设置线程ID
		t->Start();											//启动线程
		threads.push_back(t);								//添加到线程列表
		this_thread::sleep_for(chrono::milliseconds(100));	//休眠100ms
	}
}