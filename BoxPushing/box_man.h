#pragma once
#ifndef _BOX_MAN_H
#define	_BOX_MAN_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 650
#define LINE 9
#define COLUMN 12
#define RATIO 50
#define START_X 100
#define START_Y 100

typedef enum _DIRECTION DIRECTION;	//自定义类型
typedef struct _POS POS;
typedef enum _PROPS PROPS;

//keyboard control

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_QUIT 'q'

#define isValid(POS) ((POS.x >= 0) && (POS.x < LINE) && (POS.y >= 0) && (POS.y < COLUMN))	//宏定义函数

enum _PROPS {
	WALL,
	FLOOR,
	BOX_DES,
	MAN,
	BOX,
	HIT,
	MAN_IN_DES,
	ALL
};

enum _DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

struct _POS {
	int x;
	int y;
};

bool isGameOver();
//void gameOverScene(IMAGE* bg);
void ChangeMap(POS* pos, PROPS prop);
void gameControl(DIRECTION direct);

#endif 