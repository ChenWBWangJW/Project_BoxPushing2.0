#pragma once
#include "DatabaseTask.h"
#include <string>
using namespace std;

class UserInfo : public DatabaseTask
{
public:
	void Parse(string);

	void Read(bufferevent*);

	void Write(bufferevent*);

	void Event(bufferevent*, short);

	bool ConnecToDB();

	void GetGameData();

	virtual bool Init();

	UserInfo();
	~UserInfo();
private:
	string buf;
};

