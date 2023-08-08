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
		return &pool;			//�����̳߳ض���ĵ�ַ
	}

	void Init(int thread_num);	//��ʼ���̳߳�

	void Dispatch(Task* );	//�ַ�����

private:
	int thread_num;			//�߳�����
	int thread_index = -1;		//�߳�����,���һ���̵߳�λ��
	std::vector<Thread*> threads;	//�߳��б�
	ThreadPool() {};			//���캯��˽�л�
};