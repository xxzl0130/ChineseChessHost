#include <Wire\Wire.h>
#include <LiquidCrystal_I2C\LiquidCrystal_I2C.h>
#include <String.h>
#include "CommonDef.h"
#include "SlaveDef.h"
#include "StepperMotor.h"
#include "SlaveMain.h"
#include "MatrixKeyboard.h"
#include "ChessBoard.h"
#include "SlipTable.h"
/*
Todo list:

###调整求和、认输逻辑
###中断中无法使用串口

// 开始
void start();
// 游戏进行
void playing(); // 需要调整
// 重置棋盘
void reset();

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
// 游戏状态 0 正常，1 求和，2 认输
GameState state = Play;
// 棋盘
ChessBoard board;

// 检测人移动棋子
bool humanMoveChess();
// 执行命令
void executeOrder(String& order);
// 求和 flag:false 人 true 机
bool draw(bool flag);
// 中断用函数 调用draw
void callDraw();
// 中断用函数 调用resign
void callResign();
// 认输 flag:false 人 true 机
bool resign(bool flag);
// 等待开始
void waitStart();
// 选择难度
void selectDiff();
// 选择顺序
void selectOrder();
// 开始
void start();
// 游戏进行
void playing();
// 重置棋盘
void reset();
// 结束游戏
void end(GameState state);

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
	playing();
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
	board.moveChess(move);
}

bool draw(bool flag)
{
	++drawCnt;
	if (drawCnt >= 2)
	{// 双方同意和棋
		drawCnt = 0;
		end(Draw);
		return true;
	}
	if (flag == false)
	{// 人提和
		sendBoard(board.board,Draw);
	}
	else
	{// 引擎提和
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer draw.");
		Lcd.setCursor(0, 1);
		Lcd.print(" Agree  Reject  ");
		while(true)
		{
			if(Serial2.available())
			{
				auto t = Serial.read();
				if(t == StartKey)
				{
					draw(false);
					break;
				}
				else if(t == EndKey)
				{
					Lcd.setCursor(0, 1);
					Lcd.print(" You Rejected.");
					drawCnt = 0;
					delay(1000);
					break;
				}
			}
		}
	}
	return false;
}

void callDraw()
{
	if(isPress(StartKey,LOW))
	{
		draw(false);
	}
}

void callResign()
{
	if (isPress(EndKey, LOW))
	{
		resign(false);
	}
}

bool resign(bool flag)
{
	static bool saveFlag;
	++resignCnt;
	if (resignCnt >= 2)
	{// 双方同意认输
		resignCnt = 0;
		if(saveFlag == false)
		{
			end(Lose);
		}
		else
		{
			end(Win);
		}
		return true;
	}
	if (flag == false)
	{// 人认输
		sendBoard(board.board,Resign);
	}
	else
	{// 引擎认输
		Lcd.clear();
		Lcd.setCursor(0, 0);
		Lcd.print("Computer resign.");
		Lcd.setCursor(0, 1);
		Lcd.print("CONGRATUALTIONS!");
		delay(1000);
		end(Win);
	}
	saveFlag = flag;
	return false;
}

void waitStart()
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	Lcd.print(" Chinese Chess");
	Lcd.setCursor(0, 1);
	Lcd.print("Press Start key");
	// 等待按下开始按钮
	while (true)
	{
		if(Serial2.available())
		{
			auto t = Serial2.read();
			if(t == StartKey)
			{
				break;
			}
		}
	}
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
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey)
			{
				// 难度选择完成
				break;
			}
			if (t == LeftKey)
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
			if (t == RightKey, LOW)
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
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey)
			{
				// 先后手选择完成
				break;
			}
			if (t == LeftKey)
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
			if (t == RightKey)
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
}

void start()
{
}

void playing()
{
	while(true)
	{
		if (humanMoveChess())
		{
			sendBoard(board.board);
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
}

void reset()
{
}

void end(GameState state)
{
	Lcd.clear();
	Lcd.setCursor(0, 0);
	// 显示提示信息
	switch(state)
	{
	case Draw:
		Lcd.print("Game End!Draw!");
		break;
	case Win:
		Lcd.print("You Win!");
		break;
	case Lose:
		Lcd.print("You lose!");
		break;
	default:
		break;
	}
	Lcd.setCursor(0, 1);
	Lcd.print("Play again?");
	while(true)
	{
		if (Serial2.available())
		{
			auto t = Serial2.read();
			if (t == StartKey, LOW)
			{
				reset();
				break;
			}
		}
	}
}