#pragma once
#include <vector>

class Task;
class Thread;
class ThreadPool
{
public:
	static ThreadPool* Get()
	{
		static ThreadPool pool;
		return &pool;			//返回线程池对象的地址
	}

	void Init(int thread_num);	//初始化线程池

	void Dispatch(Task* );	//分发任务

private:
	int thread_num;			//线程数量
	int thread_index = -1;		//线程索引,最后一个线程的位置
	std::vector<Thread*> threads;	//线程列表
	ThreadPool() {};			//构造函数私有化
};