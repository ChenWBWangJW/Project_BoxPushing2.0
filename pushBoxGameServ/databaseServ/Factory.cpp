#include "Factory.h"
#include "UserInfo.h"
#include "testUtil.h"

//���������������д������ļ������

Task* Factory::CreateTask()
{
	perrorText("At XFtpFactory::CreateTask");

	//����һ���������ݿ����������ʵ������
	UserInfo* task = new UserInfo();
	

	return task;
}

Factory::Factory()
{
}