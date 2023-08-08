#include "userDB.h"
#include <stdio.h>

static int debug = 1;

static bool connect_db(MYSQL &mysql);

bool login_db(const cmdInfo& CMD)
{
	MYSQL mysql;
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	char sql[SIZE_OF_SQL] = "";
	bool ret = false;

	if ((connect_db(mysql)) == false)
	{
		perrorText("Connect to database failed.");
		return false;
	}
	else
	{
		if(debug) printf("Connect to database success.\n");
	}

	snprintf(sql, SIZE_OF_SQL, "select * from users where username='%s' and password='%s';", CMD.username.c_str(), CMD.password.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		printf("return ret :%d, Reason: %s\n", ret, mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL query success.\n");
	}

	mysql_res = mysql_store_result(&mysql);
	if (mysql_res == NULL)
	{
		perrorText("MYSQL store result error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL store result success.\n");
	}

	mysql_row = mysql_fetch_row(mysql_res);
	if (mysql_row == NULL)
	{
		perrorText("MYSQL fetch row error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL fetch row success.\n");
	}
	
	mysql_close(&mysql);
	return true;
}


bool register_db(const cmdInfo& CMD)
{
	MYSQL mysql;
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	char sql[SIZE_OF_SQL] = "";
	bool ret = false;

	if ((connect_db(mysql)) == false)
	{
		perrorText("Connect to database failed.");
		return false;
	}
	else
	{
		if (debug) printf("Connect to database success.\n");
	}

	sprintf(sql, "insert into users(username, password) values('%s', '%s');", CMD.username.c_str(), CMD.password.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		puts("in func register_db:");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL query success.\n");
	}

	mysql_res = mysql_store_result(&mysql);
	if (mysql_res == NULL)
	{
		perrorText("MYSQL store result error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL store result success.\n");
	}

	mysql_row = mysql_fetch_row(mysql_res);
	if (mysql_row == NULL)
	{
		perrorText("MYSQL fetch row error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL fetch row success.\n");
	}

	mysql_close(&mysql);
	return true;
}

bool delele_acount_db(const cmdInfo& CMD)
{
	MYSQL mysql;
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	char sql[SIZE_OF_SQL] = "";
	bool ret = false;

	if ((connect_db(mysql)) == false)
	{
		perrorText("Connect to database failed.");
		return false;
	}
	else
	{
		if (debug) printf("Connect to database success.\n");
	}

	sprintf(sql, "delete from users where username='%s' and password='%s';", CMD.username.c_str(), CMD.password.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL query success.\n");
	}

	mysql_res = mysql_store_result(&mysql);
	if (mysql_res == NULL)
	{
		perrorText("MYSQL store result error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL store result success.\n");
	}

	mysql_row = mysql_fetch_row(mysql_res);
	if (mysql_row == NULL)
	{
		perrorText("MYSQL fetch row error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL fetch row success.\n");
	}

	mysql_close(&mysql);
	return true;
}

bool get_user_info_db(cmdInfo& CMD)
{
	MYSQL mysql;
	MYSQL_RES* mysql_res;
	MYSQL_ROW mysql_row;
	char sql[SIZE_OF_SQL] = "";
	bool ret = false;

	if ((connect_db(mysql)) == false)
	{
		perrorText("Connect to database failed.");
		return false;
	}
	else
	{
		if (debug) printf("Connect to database success.\n");
	}

	sprintf(sql, "select id, level_id from users where username = '%s';", CMD.username.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL query success.\n");
	}

	mysql_res = mysql_store_result(&mysql);
	if (mysql_res == NULL)
	{
		perrorText("MYSQL store result error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL store result success.\n");
	}

	mysql_row = mysql_fetch_row(mysql_res);
	if (mysql_row == NULL)
	{
		perrorText("MYSQL fetch row error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_res);
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL fetch row success.\n");
	}

	CMD.id = mysql_row[0];
	CMD.level_id = mysql_row[1];

	mysql_close(&mysql);
	return true;
}

bool update_user_level(cmdInfo& CMD)
{
	//1.连接数据库
	MYSQL mysql;
	char sql[SIZE_OF_SQL];
	bool ret = false;

	if ((connect_db(mysql)) == false)
	{
		perrorText("Connect to database failed.");
		return false;
	}
	else
	{
		if (debug) printf("Connect to database success.\n");
	}

	//2.更新用户关卡信息
	snprintf(sql, SIZE_OF_SQL, "update users set level_id = %s where id = %s;", CMD.next_level_id.c_str(), CMD.id.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret)
	{
		perrorText("MYSQL query error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("MYSQL query success.\n");
	}

	//3.关闭数据库
	mysql_close(&mysql);
	return true;
}

bool connect_db(MYSQL& mysql)
{
	if ((mysql_init(&mysql)) == NULL)
	{
		perrorText("MYsql init error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		if (debug) printf("Mysql init success.\n");
	}

	if ((mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) == NULL)
	{
		perrorText("Mysql connect error.");
		printf("Reason: %s\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		if (debug) printf("Mysql connect success.\n");
	}

	return true;
}
