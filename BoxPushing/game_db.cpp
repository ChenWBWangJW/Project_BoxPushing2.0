#include "game_db.h"
#include<mysql.h>
#include<stdio.h>

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_USER_PASSWD "chenweibin980928."
#define DB_NAME "BOX_MAN"
#define DB_PORT 3306

#define SIZE_OF_SQL 256

static int debug = 1;
static bool connect_db(MYSQL &mysql);


/************************************
* 函数名：fetch_user_info
* 功能：通过数据库验证用户身份,获取用户信息
* 输入：
*	user - 用户信息结构体
* 返回值：
*	bool - true为验证成功，false为验证失败
*************************************/
bool fetch_user_info(userinfo& user)
{
	MYSQL mysql;					//mysql句柄，官方结构体
	MYSQL_RES* mysql_result;		//查询结果集
	MYSQL_ROW mysql_row;			//结果集中的一行

	char sql[SIZE_OF_SQL];
	bool ret = false;
	
	//1.连接数据库
	if (connect_db(mysql) == false)
	{
		return false;
	}

	//2.根据用户名和密码查询获取用户信息（id， level_id）
	//将可变参数 “…” 按照format的格式格式化为字符串，然后再将其拷贝至str中
	//c_str()将C++字符串对象转换为C风格的字符串，该函数返回一个指向以空字符结尾的字符数组的指针，该字符数组包含了C++字符串对象中的字符序列
	snprintf(sql, SIZE_OF_SQL, "select id, level_id from users where username = '%s' and password = md5('%s');", user.username.c_str(), user.password.c_str());
	ret = mysql_query(&mysql, sql);		//查询成功返回值为0，否则为非0

	if (ret)
	{
		printf("用户数据查询错误: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("用户数据查询成功......\n");
	}

	//3.获取查询结果
	mysql_result = mysql_store_result(&mysql);
	if (mysql_result == NULL)
	{						//预防空指针报错
		printf("用户数据载入错误, reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_result);			//释放结果集
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("用户数据载入成功......\n");
	}

	mysql_row = mysql_fetch_row(mysql_result);		//获取结果集中的一行
	if (mysql_row == NULL)
	{
		printf("用户数据查获获取结果集错误, reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_result);			//释放结果集
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("用户数据获取成功......\n");
	}

	//4.返回结果
	/*
	atoi()是一个C标准库函数，用于将字符串转换成整数
	如果字符串中包含非数字字符，则atoi()函数会停止转换，并返回已经转换的部分
	此外，在使用atoi()函数时，需要确保要转换的字符串表示的整数不超过int类型的范围，否则会导致结果不正确
	*/
	user.id = atoi(mysql_row[0]);
	user.level_id = atoi(mysql_row[1]);
	if(debug) printf("用户号 ： %d, 地图号 ： %d\n", user.id, user.level_id);

	//关闭结果集
	mysql_free_result(mysql_result);			//释放结果集
	//关闭数据库
	mysql_close(&mysql);

	return true;
}


/************************************
* 函数名：connetc_db
* 功能：标准化连接数据库
* 输入：
*	对mysql句柄的引用
* 返回值：
* 	bool - true为连接成功，false为连接失败
*************************************/
bool connect_db(MYSQL &mysql) {
	//1.初始化数据库句柄
	mysql_init(&mysql);

	//2.设置字符编码
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	//3.连接数据库
	if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWD, DB_NAME, DB_PORT, NULL, 0) != NULL) {
		if (debug) printf("数据库连接成功......\n");
	}
	else {
		printf("数据库连接错误:%s\n", mysql_error(&mysql));
		return false;
	}


	return true;
}


/************************************
* 函数名：fetch_level_info
* 功能：通过数据库获取关卡信息
* 输入：
*	level - 关卡信息结构体
*	level_id - 与用户关联关卡id
* 返回值：
*	bool - true为获取成功，false为获取失败
*************************************/
bool fetch_level_info(levelinfo &level, int level_id) {
	MYSQL mysql;					//mysql句柄，官方结构体
	MYSQL_ROW mysql_row;			//结果集中的一行
	MYSQL_RES* mysql_result;		//查询结果集
	char sql[SIZE_OF_SQL];
	bool ret = false;

	//1.连接数据库
	if (connect_db(mysql) == false) {
		return false;
	}

	//2.根据关卡id查询获取关卡信息
	//将可变参数 “…” 按照format的格式格式化为字符串，然后再将其拷贝至str中
	snprintf(sql, SIZE_OF_SQL, "select name, map_row, map_column, map_data, next_level_id from levels where id = '%d';", level_id);
	ret = mysql_query(&mysql, sql);		//查询成功返回值为0，否则为非0
	if (ret)
	{
		printf("地图数据查询错误, reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("地图数据查询成功......\n");
	}

	//3.获取查询结果
	mysql_result = mysql_store_result(&mysql);
	if(mysql_result == NULL)		//预防空指针报错
	{
		printf("地图数据载入错误, reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_result);			//释放结果集
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("地图数据载入成功......\n");
	}

	mysql_row = mysql_fetch_row(mysql_result);		//获取结果集中的一行
	if (mysql_row == NULL)
	{
		printf("地图数据获取失败, reason: %s\n", mysql_error(&mysql));
		mysql_free_result(mysql_result);			//释放结果集
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("地图数据获取成功......\n");
	}

	//4.返回结果
	level.id = level_id;
	level.name = mysql_row[0];
	level.map_row = atoi(mysql_row[1]);
	level.map_col = atoi(mysql_row[2]);
	level.map_data = mysql_row[3];
	level.next_level_id = atoi(mysql_row[4]);
	if (debug) printf("地图数据已准备就绪，地图名： %s\n", level.name.c_str());

	//关闭结果集
	mysql_free_result(mysql_result);			//释放结果集

	//关闭数据库
	mysql_close(&mysql);

	return true;
}


/************************************
* 函数名：transfrom_map_db2array
* 功能：将数据库中的地图数据转换为数组
* 输入：
*	level - 关卡信息结构体
*	map - 二维数组
* 返回值：
*	true - 转换成功
*	false - 转换失败
*************************************/
bool transfrom_map_db2array(levelinfo& level, int map[LINE][COLUMN])
{
	if (level.map_row > LINE || level.map_col > COLUMN)
	{
		printf("地图数据错误，地图行数或列数超出范围\n");
		return false;
	}

	if (level.map_data.length() < 1)
	{
		printf("地图数据错误，请重新设置\n");
		return false;
	}

	int start = 0, end = 0;
	int row = 0, column = 0;
	
	do {
		/*
		.find()是一个字符串类型的成员函数，用于在字符串中查找指定的子串
		.find()函数接受一个参数，即要查找的子串。如果找到了该子串，则返回该子串在原字符串中的起始位置；否则返回string::npos，表示未找到
		*/
		end = level.map_data.find("|", start);

		if (end < 0)
		{
			end = level.map_data.length();
			break;
		}

		if (start == end) break;

		/*
		是一个字符串类型的成员函数，用于获取字符串的子串
		.substr()函数接受两个参数，第一个参数是子串的起始位置，第二个参数是子串的长度
		如果只传递一个参数，则表示获取从起始位置到字符串末尾的子串
		需要包含头文件<string.h>
		*/
		string line = level.map_data.substr(start, end - start);

		/*
		解析每一行
		strtok_s是一个字符串处理函数，用于将一个字符串分割成多个子串
		str是要分割的字符串，delim是分隔符，context是一个指向指针的指针，用于保存分割的状态
		需要包含头文件<string.h>
		//0,0,0,0,0,0,0,0,0,0,0,0|0,1,0,1,1,1,1,1,1,1,0,0|0,1,4,1,0,2,1,0,2,1,0,0........
		*/
		char *next_token = NULL;
		char* item = strtok_s((char *)line.c_str(), ",", &next_token);

		column = 0;

		while (item && column < level.map_col)
		{
			map[row][column] = atoi(item);
			column++;
			item = strtok_s(NULL, ",", &next_token);
		}

		if(column < level.map_col)
		{
			printf("地图数据错误，地图列数不足\n");
			return false;
		}

		row++;

		if (row > level.map_row) break;

		start = end + 1;
	} while (true);

	if (row < level.map_row)
	{
		printf("地图行数少于设定， %d（need：%d）\n", row, level.map_row);
		return false;
	}
}

/************************************
* 函数名：update_user_level
* 功能：更新用户关卡信息
* 输入：
*	user - 用户信息结构体
*	next_level_id - 下一关卡ID
* 返回值：
*	true - 更新成功
*	false - 更新失败
*************************************/
bool update_user_level(userinfo& user, int next_level_id)
{
	//1.连接数据库
	MYSQL mysql;
	char sql[SIZE_OF_SQL];

	if (!(connect_db(mysql)))
	{
		printf("数据库连接失败\n");
		return false;
	}

	//2.更新用户关卡信息
	snprintf(sql, SIZE_OF_SQL, "update users set level_id = %d where id = %d;", next_level_id, user.id);
	if (mysql_query(&mysql, sql))
	{
		printf("数据库更新失败，reason: %s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	else
	{
		if (debug) printf("用户关卡信息更新成功......\n");
	}

	//3.关闭数据库
	mysql_close(&mysql);
	return true;
}
