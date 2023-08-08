#pragma once
#include "Task.h"

class Factory
{
public:

	static Factory* Get()
	{
		static Factory f;
		return &f;
	}


	Task* CreateTask();

private:
	Factory();
};
