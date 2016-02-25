#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StpperMotor.h"
#include "SlaveMain.h"
/*
Todo list:
// 初始化棋盘，设置难度、先手等
void initBoard();

// 检测人移动棋子
bool humanMoveChess();

显示屏控制

// 滑台移动棋子
void moveChess(char order[4]);
*/

String tmp;
char buf[MAX_BUF_SIZE];
// 对局回合数
ulong roundCnt;
// 电脑执子
char AIColor;
// 提和计数
uchr drawCnt;
// 认输计数
uchr resignCnt;
// 难度
DIFFICULTY diff;
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

// 初始化串口
bool initSerial();
// 初始化棋盘，设置难度、先手等
void initBoard();
// 检测人移动棋子
bool humanMoveChess();
// 从主机接收命令
String readOrderFromHost();
// 执行命令
void executeOrder(String& order);
// 求和 flag:false 人 true 机
bool draw(bool flag);
// 认输 flag:false 人 true 机
bool resign(bool flag);
// 发送go（思考）命令。state:0 正常，1 求和，2 认输
void sendGo(uchr state = 0);
// 发送棋盘描述。state:0 正常，1 求和，2 认输
void sendBoard(uchr state = 0);
// 滑台移动棋子
void moveChess(char order[4]);

void setup()
{
	pinMode(ledPin, OUTPUT);
	digitalWrite(13, LOW);
	initSerial();
	initBoard();
}

void loop()
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

bool initSerial()
{
	comSer.begin(baudRate);
	if (!comSer)
		return false;
	for (uchr i = 0; i < 5; ++i)
	{
		tmp = comSer.readString();
		//comSer.println(tmp);
		if (strstr(tmp.c_str(), testComHost) != NULL)
		{ // 找到应有字符串，表示连接成功
			comSer.println(testComSlave); // 回复
			return true;
		}
		delay(100);
	}
	return false;
}

void initBoard()
{
}

bool humanMoveChess()
{
	return true;
}

String readOrderFromHost()
{
	while (true)
	{
		if (comSer.available())
		{
			tmp = comSer.readString();
			if (strstr(tmp.c_str(), "bestmove") != NULL)
			{ // 找到bestmove
				return tmp;
			}
		}
		delay(100);
	}
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
	char* ptr = strstr(order.c_str(), "bestmove") + 9/*跳过"bestmoove "*/;
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

void sendGo(uchr state)
{
	comSer.print("go depth");
	comSer.print(diff);
	if (state == 1)
	{// 提和
		comSer.print(" draw");
	}
	else if (state == 2)
	{
		comSer.print(" resign");
	}
	comSer.print('\n');
}

void sendBoard(uchr state)
{
	creatFEN(board, buf, AIColor, ++roundCnt);
	comSer.println(buf);
	sendGo(state);
}

void moveChess(char order[4])
{
}
