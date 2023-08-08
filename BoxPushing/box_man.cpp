#include<iostream>
#include<graphics.h>	//图形化easyx库头文件
#include<conio.h>		//按键获取头文件
#include<stdlib.h>		//标准库头文件
#include<string.h>		//字符串相关头文件
#include<cstdlib>

using namespace std;

#include "box_man.h"
#include "connecToServer.h"

#define LINE 48
#define COLUMN 48

IMAGE images[ALL];	//easyx库函结构类型调用

POS man;	//man position;

int map[LINE][COLUMN] = { 0 };	//map data]

/*
int map[LINE][COLUMN] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
	{0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, },
	{0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 0, 0, },
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, },
	{0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0, },
	{0, 1, 1, 1, 0, 3, 1, 1, 1, 4, 1, 0, },
	{0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0, },
	{0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, },
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};
*/

/*************************************************
* 函数名：isGameOver
* 实现游戏结束判断
* 输入：无
* 输出：无
* 返回值：
	true - 游戏结束
* 	false - 游戏未结束
**************************************************/
bool isGameOver() {
	for (int i = 0; i < LINE; i++) {
		for (int j = 0; j < COLUMN; j++) {
			if (map[i][j] == BOX_DES) {
				return false;
			}
		}
	}
	return true;
}

void game_Over_Scene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	settextstyle(20, 0, "宋体");
	drawtext("恭喜你！！！挑战成功！！！\n请敲任意键两次跳转下一关", &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void game_error_Over_Scene(IMAGE* bg) {
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	settextstyle(20, 0, "宋体");
	drawtext("游戏地图仅更新到此，\n轻敲任意键退出游戏", &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/*************************************************
* 函数名：ChangeMap
* 实现地图变化
* 输入：
*	*pos - 人当前位置
*	prop - 替换地图图片展示
* 输出：无
* 返回值：无
**************************************************/
void ChangeMap(POS* pos, PROPS prop) {
	map[pos->x][pos->y] = prop;
	putimage(START_X + (pos->y * RATIO), START_Y + (pos->x * RATIO), &images[prop]);
}

/*************************************************
* 函数名：gameControl
* 实现游戏四个方向控制
* 输入：
*	direct - 人前进方向
* 输出：无
* 返回值：无
**************************************************/
void gameControl(DIRECTION direct) {
	POS next_pos = man, next_next_pos = man;

	switch (direct) {
	case UP:
		next_pos.x--;
		next_next_pos.x = man.x - 2;
		break;

	case DOWN:
		next_pos.x++;
		next_next_pos.x = man.x + 2;
		break;

	case LEFT:
		next_pos.y--;
		next_next_pos.y = man.y - 2;
		break;

	case RIGHT:
		next_pos.y++;
		next_next_pos.y = man.y + 2;
		break;

	}

	//1.判断前进位置是否为地板，true则前进，原位置恢复目的地
/*	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR && map[man.x][man.y] == MAN_IN_DES) {
		ChangeMap(&next_pos, MAN);
		ChangeMap(&man, BOX_DES);
		man = next_pos;
	} */

	/*	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX_DES) {
			ChangeMap(&next_pos, MAN_IN_DES);
			ChangeMap(&man, FLOOR);
			man = next_pos;
		} */

		//1.判断前进位置是否为地板，true则前进，原位置恢复地板
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR) {
		ChangeMap(&next_pos, MAN);
		ChangeMap(&man, FLOOR);
		man = next_pos;
	}

	//2.判断前进位置是否为箱子，true则判断箱子下一位置next_next_pos是否为地板，是则箱子前进，判断是否为箱子目的地，人站到箱子位置，原位置恢复地板
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX) {

		//2.1箱子前是地板
		if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == FLOOR) {
			ChangeMap(&next_next_pos, BOX);
			ChangeMap(&next_pos, MAN);
			ChangeMap(&man, FLOOR);
			man = next_pos;
		}

		//2.1箱子前面是目的地
		else if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == BOX_DES) {
			ChangeMap(&next_next_pos, BOX);
			ChangeMap(&next_pos, MAN);
			ChangeMap(&man, FLOOR);
			man = next_pos;
		}
	}
}


void init_game_graphic(IMAGE &bg_img) {
	

	//搭台唱戏
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);

	//设置背景
	loadimage(&bg_img, _T("picture/blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);	//_T()为字符转换
	putimage(0, 0, &bg_img);

	//加载图片(墙0， 地板1， 箱子目的地2， 小人3， 箱子4， 箱子命中目标5)
	loadimage(&images[WALL], _T("picture/wall_right.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("picture/floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("picture/des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("picture/man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("picture/box.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("picture/box.bmp"), RATIO, RATIO, true);
	//	loadimage(&images[MAN_IN_DES], _T("man_in_des.bmp"), RATIO, RATIO, true);
}

void main(void)
{
	//0.用户身份验证
	userinfo user;
	levelinfo level;
	IMAGE bg_img;
	bool ret = false;

	if (!login(user)) {
		cout << "验证失败，程序退出" << endl;
		exit(-1);
	}
	else {
		cout << "验证成功." << endl;
		printf("用户号 ： %d, 地图号 ： %d\n", user.id, user.level_id);
		::system, ("pause");
	}
	
	//游戏主循环
	bool quit = false;

	do {
		//根据用户所在的关卡id获取关卡数据
		ret = fetch_level_info(user, level, user.level_id);
		if (ret)
		{
			cout << "地图展开中......" << endl;
			::system("pause");
		}
		else {
			cout << "获取关卡数据失败" << endl;
			game_error_Over_Scene(&bg_img);
			break;
		}

		init_game_graphic(bg_img);

		//把数据库地图数据转换到map中
		ret = transfrom_map_db2array(level, map);
		if (!ret)
		{
			cout << "地图转换错误" << endl;
			exit(-1);
		}

		for (int i = 0; i < level.map_row; i++) {
			for (int j = 0; j < level.map_col; j++) {
				if (map[i][j] == MAN) {
					man.x = i;
					man.y = j;
				}
				putimage(START_X + (j * RATIO), START_Y + (i * RATIO), &images[map[i][j]]);
			}
		}

		do {
			if (_kbhit()) {
				char ch = _getch();

				if (ch == KEY_UP) {
					gameControl(UP);

				}
				else if (ch == KEY_DOWN) {
					gameControl(DOWN);

				}
				else if (ch == KEY_LEFT) {
					gameControl(LEFT);

				}
				else if (ch == KEY_RIGHT) {
					gameControl(RIGHT);

				}
				else if (ch == KEY_QUIT) {
					quit = true;
				}

				if (isGameOver()) {

					if(level.next_level_id == 0)
					{
						quit = true;
						game_error_Over_Scene(&bg_img);
						break;
					}
					else 
					{
						user.level_id += 1;
						update_user_level(user, user.level_id);
						game_Over_Scene(&bg_img);
						break;
					}
				}
			}
			Sleep(100);
		} while (quit == false);	//!quit
	} while (quit == false);

	user.level_id = 1;
	update_user_level(user, user.level_id);

	exit(-1);
}