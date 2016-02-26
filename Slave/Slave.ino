#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StpperMotor.h"
#include "SlaveMain.h"
#include "MatrixKeyboard.h"
/*
Todo list:
// 初始化棋盘
void initBoard();

// 检测人移动棋子
bool humanMoveChess();

// 滑台移动棋子
void moveChess(char order[4]);
*/

//LCD1602
LiquidCrystal_I2C Lcd(0x27, 16, 2);
/*
显示屏样式：
 0123456789012345
 ******************
0*                *0x80
1*                *0xC0
 ******************
    >Red<  Black  
*/

String tmp;
char buf[MAX_BUF_SIZE];
// 对局回合数
ulong roundCnt;
// 电脑执子
char AIColor = 'b';
// 提和计数
uchr drawCnt;
// 认输计数
uchr resignCnt;
// 难度
DIFFICULTY diff = easy;
// 棋盘
char board[BoardRow][BoardCol] = {
	{ r,h,e,a,k,a,e,h,r },
	{ b,b,b,b,b,b,b,b,b },
	{ b,c,b,b,b,b,b,c,b },
	{ p,b,p,b,p,b,p,b,p },
	{ b,b,b,b,b,b,b,b,b },
	{ b,b,b,b,b,b,b,b,b },
	{ P,b,P,b,P,b,P,b,P },
	{ b,C,b,b,b,b,b,C,b },
	{ b,b,b,b,b,b,b,b,b },
	{ R,H,E,A,K,A,E,H,R }
};

// 检测人移动棋子
bool humanMoveChess();
// 执行命令
void executeOrder(String& order);
// 求和 flag:false 人 true 机
bool draw(bool flag);
// 认输 flag:false 人 true 机
bool resign(bool flag);
// 滑台移动棋子
void moveChess(char order[4]);
// 等待开始
void waitStart();
// 选择难度
void selectDiff();
// 选择顺序
void selectOrder();
// 开始
void start();
// 游戏进行
void play();
// 恢复棋盘
void reset();

void setup()
{
	initPin();
	initLCD();
	initSerial();
	initBoard();
}

void loop()
{
	waitStart();
	start();
	play();
	reset();
}

bool humanMoveChess()
{
	return true;
}

void executeOrder(String& order)
{
	if (strstr(order.c_str(), "draw") != NULL)
	{
		// 引擎提和
		if (draw(true))
		{
			return;
		}
	}
	else
	{// 没有提和 清空计数
		drawCnt = 0;
	}
	if (strstr(order.c_str(), "resign") != NULL)
	{
		// 引擎认输
		if (resign(true))
		{
			return;
		}
	}
	else
	{// 没有认输 清空计数
		resignCnt = 0;
	}
	// 提取4位的走子信息
	char move[4];
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*跳过"bestmove "*/;
	for (uint i = 0; i < 4; ++i)
	{
		move[i] = ptr[i];
	}
	// 走子
	moveChess(move);
}

bool draw(bool flag)
{
	++drawCnt;
	if (drawCnt >= 2)
	{// 双方同意和棋
		drawCnt = 0;
		// todo: 显示屏显示和棋信息
		return true;
	}
	if (flag == false)
	{// 人提和
		sendBoard(1);
	}
	else
	{// 引擎提和
		// todo: 显示屏显示和棋信息
	}
	return false;
}

bool resign(bool flag)
{
	++resignCnt;
	if (resignCnt >= 2)
	{// 双方同意认输
		resignCnt = 0;
		// todo: 显示屏显示和棋信息
		return true;
	}
	if (flag == false)
	{// 人认输
		sendBoard(2);
	}
	else
	{// 引擎提和
		// todo: 显示屏显示和棋信息
	}
	return false;
}

void moveChess(char order[4])
{
}

void waitStart()
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	// 等待按下开始按钮
	while (!isPress(StartKey));
	selectDiff();
	selectOrder();
}

void selectDiff()
{
	// 选择难度
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" >Easy< Normal");
	Lcd.setCursor(0, 1);
	Lcd.print("  Hard  Master");
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			// 难度选择完成
			break;
		}
		if (isPress(LeftKey, LOW))
		{
			switch (diff)
			{
			case easy:
				diff = easy;
				break;
			case normal:
				Lcd.setCursor(0, 0);
				Lcd.print(" >Easy< Normal ");
				diff = easy;
				break;
			case hard:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy >Normal<");
				diff = normal;
				break;
			case master:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				break;
			}
		}
		if (isPress(RightKey, LOW))
		{
			switch (diff)
			{
			case easy:
				Lcd.setCursor(0, 0);
				Lcd.print("  Easy >Normal<");
				diff = normal;
				break;
			case normal:
				Lcd.setCursor(0, 1);
				Lcd.print(" >Hard< Master ");
				diff = hard;
				break;
			case hard:
				Lcd.setCursor(0, 1);
				Lcd.print("  Hard >Master<");
				diff = master;
				break;
			case master:
				diff = master;
				break;
			}
		}
	}
}

void selectOrder()
{
	// 选择先后手
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print("Select Color:");
	Lcd.setCursor(0, 1);
	Lcd.print("  >Red<  Black  ");
	while (true)
	{
		if (isPress(StartKey, LOW))
		{
			// 先后手选择完成
			break;
		}
		if (isPress(LeftKey, LOW))
		{
			switch (AIColor)
			{
			case 'b':
				AIColor = 'b';
				break;
			case 'r':
				Lcd.setCursor(0, 1);
				Lcd.print("  >Red<  Black  ");
				AIColor = 'b';
				break;
			default:
				AIColor = 'b';
				break;
			}
		}
		if (isPress(RightKey, LOW))
		{
			switch (AIColor)
			{
			case 'b':
				Lcd.setCursor(0, 1);
				Lcd.print("   Red  >Black< ");
				AIColor = 'r';
				break;
			case 'r':
				AIColor = 'b';
				break;
			default:
				AIColor = 'r';
				break;
			}
		}
	}
}

void start()
{
}

void play()
{
	if (humanMoveChess())
	{
		sendBoard();
		delay(500);
		tmp = readOrderFromHost();
		executeOrder(tmp);
	}
	if (comSer.available())
	{
		tmp = readOrderFromHost();
		executeOrder(tmp);
	}
	delay(100);
}

void reset()
{
}
