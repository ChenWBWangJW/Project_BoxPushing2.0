#pragma once
#ifndef USER_DB
#define USER_DB

#include "testUtil.h"
#include "sockConn.h"

#include <iostream>
#include <cstring>
#include <mysql.h>
#include <time.h>

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "chenweibin980928."
#define DB_NAME "gamedb"

#define SIZE_OF_SQL 256

bool login_db(const cmdInfo& CMD);
bool register_db(const cmdInfo& CMD);
bool delele_acount_db(const cmdInfo& CMD);
bool get_user_info_db(cmdInfo& CMD);
bool update_user_level(cmdInfo& CMD);

#endif // !USER_DB

