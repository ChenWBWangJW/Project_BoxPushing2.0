#include "Factory.h"
#include "UserInfo.h"
#include "testUtil.h"

//后续有新需求可再写完相关文件后加入

Task* Factory::CreateTask()
{
	perrorText("At XFtpFactory::CreateTask");

	//创建一个传输数据库数据任务的实例对象
	UserInfo* task = new UserInfo();
	

	return task;
}

Factory::Factory()
{
}